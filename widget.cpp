/*
* Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
*
*/
#include "widget.h"
#include "ui_widget.h"
#include "notewidgetdelegate.h"
#include "edit_page.h"

#define FIRST_LINE_MAX 80

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
  , m_notebook(new Edit_page(this))
  , m_autoSaveTimer(new QTimer(this))
  , m_settingsDatabase(Q_NULLPTR)
  , m_ukui_SearchLine(Q_NULLPTR)
  , m_newKynote(Q_NULLPTR)
  , m_trashButton(Q_NULLPTR)
  , m_countLabel(Q_NULLPTR)
  , m_sortLabel(Q_NULLPTR)
  , m_noteView(Q_NULLPTR)
  , m_noteModel(new NoteModel(this))
  , m_deletedNotesModel(new NoteModel(this))
  , m_proxyModel(new QSortFilterProxyModel(this))
  , m_dbManager(Q_NULLPTR)
  , m_dbThread(Q_NULLPTR)
  , m_isContentModified(false)
  , m_isColorModified(false)
{    
    translator = new QTranslator;
    QLocale locale;
    //获取系统语言环境
    if ( locale.language() == QLocale::Chinese ) {
        translator->load(QString(":/new/translation/ukui_notebook_zh_CN.qm"));  //选择翻译文件
        QApplication::installTranslator(translator);
    }
    ui->setupUi(this);
    setupDatabases();
    setupModelView();
    ukui_init();
    ukui_conn();
    QTimer::singleShot(200,this, SLOT(InitData()));
}

Widget::~Widget()
{
    for (auto it = m_editors.begin(); it!= m_editors.end();it++) {
        delete *it;
    }
    m_editors.clear();
    delete ui;
    m_dbThread->quit();
    m_dbThread->wait();
    delete m_dbThread;
}

// 初始化数据库中的数据并选中第一个便签（如果有）
void Widget::InitData()
{
    QFileInfo fi(m_settingsDatabase->fileName());
    QDir dir(fi.absolutePath());
    QString oldNoteDBPath(dir.path() + QStringLiteral("/Notes.ini"));
    QString oldTrashDBPath(dir.path() + QStringLiteral("/Trash.ini"));

    bool exist = (QFile::exists(oldNoteDBPath) || QFile::exists(oldTrashDBPath));

    if(exist){
        QProgressDialog* pd = new QProgressDialog(QStringLiteral("Migrating database, please wait."), QString(), 0, 0, this);
        pd->setCancelButton(Q_NULLPTR);
        pd->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        pd->setMinimumDuration(0);
        pd->show();

        //setButtonsAndFieldsEnabled(false);

        QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
        connect(watcher, &QFutureWatcher<void>::finished, this, [&, pd](){
            pd->deleteLater();
            //setButtonsAndFieldsEnabled(true);
            emit requestNotesList();
        });

        QFuture<void> migration = QtConcurrent::run(this, &Widget::checkMigration);
        watcher->setFuture(migration);

    } else {
        emit requestNotesList();
    }

    /// Check if it is running with an argument (ex. hide)
    if (qApp->arguments().contains(QStringLiteral("--autostart"))) {
        //setMainWindowVisibility(false);
    }
}

void Widget::setupModelView()
{
    m_noteView = static_cast<NoteView*>(ui->listView);
    m_proxyModel->setSourceModel(m_noteModel);          //代理真正的数据模型，对数据进行排序和过滤
    m_proxyModel->setFilterKeyColumn(0);                //此属性保存用于读取源模型内容的键的列,listview只有一列所以是0
    m_proxyModel->setFilterRole(NoteModel::NoteContent);//此属性保留项目角色，该角色用于在过滤项目时查询源模型的数据
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);//

    m_noteView->setItemDelegate(new NoteWidgetDelegate(m_noteView));    //安装定制delegate提供编辑功能
    m_noteView->setModel(m_proxyModel);//设置view的model是proxyModel，proxyModel作为view和QAbstractListModel的桥梁
}

void Widget::initializeSettingsDatabase()
{
    if(m_settingsDatabase->value(QStringLiteral("version"), "NULL") == "NULL")
        m_settingsDatabase->setValue(QStringLiteral("version"), qApp->applicationVersion());

    if(m_settingsDatabase->value(QStringLiteral("windowGeometry"), "NULL") == "NULL"){
        int initWidth = 704;
        int initHeight = 590;
        QPoint center = qApp->desktop()->geometry().center();
        QRect rect(center.x() - initWidth/2, center.y() - initHeight/2, initWidth, initHeight);
        setGeometry(rect);      //设置窗口居中
        m_settingsDatabase->setValue(QStringLiteral("windowGeometry"), saveGeometry());     //保存窗口的几何形状
    }

//    if(m_settingsDatabase->value(QStringLiteral("splitterSizes"), "NULL") == "NULL"){
//        m_splitter->resize(width()-2*m_layoutMargin, height()-2*m_layoutMargin);
//        QList<int> sizes = m_splitter->sizes();
//        m_noteListWidth = ui->frameLeft->minimumWidth() != 0 ? ui->frameLeft->minimumWidth() : m_noteListWidth;
//        sizes[0] = m_noteListWidth;
//        sizes[1] = m_splitter->width() - m_noteListWidth;
//        m_splitter->setSizes(sizes);
//        m_settingsDatabase->setValue(QStringLiteral("splitterSizes"), m_splitter->saveState());
//    }
}

void Widget::setupDatabases()
{
    //QSettings::IniFormat 将设置存储在INI文件中。从INI文件读取设置时不会保留类型信息。所有值将作为QString返回
    //QSettings::UserScope 将设置存储在特定于当前用户的位置
    m_settingsDatabase = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                       QStringLiteral("kylin-note"), QStringLiteral("Settings"), this);
    m_settingsDatabase->setFallbacksEnabled(false);     //禁用回退
    initializeSettingsDatabase();

    bool doCreate = false;
    QFileInfo fi(m_settingsDatabase->fileName());
    QDir dir(fi.absolutePath());
    bool folderCreated = dir.mkpath(QStringLiteral("."));
    if(!folderCreated)
        qFatal("ERROR: Can't create settings folder : %s", dir.absolutePath().toStdString().c_str());

    QString noteDBFilePath(dir.path() + QDir::separator() + QStringLiteral("kyNotes.db"));

    if(!QFile::exists(noteDBFilePath)){
        QFile noteDBFile(noteDBFilePath);
        if(!noteDBFile.open(QIODevice::WriteOnly))
            qFatal("ERROR : Can't create database file");

        noteDBFile.close();
        doCreate = true;
    }

    m_dbManager = new DBManager;
    m_dbThread = new QThread;
    m_dbThread->setObjectName(QStringLiteral("dbThread"));
    m_dbManager->moveToThread(m_dbThread);
    connect(m_dbThread, &QThread::started, [=](){emit requestOpenDBManager(noteDBFilePath, doCreate);});
    connect(this, &Widget::requestOpenDBManager, m_dbManager, &DBManager::onOpenDBManagerRequested);
    connect(m_dbThread, &QThread::finished, m_dbManager, &QObject::deleteLater);
    m_dbThread->start();
}

void Widget::error_throw()
{
    try
    {
        MY_THROW(ExceptionDerived,"error throw");
    }
    catch(ExceptionDerived &e)
    {
        std::cout << e.what() << std::endl;
    }
}

void Widget::ukui_init()
{
    sortflag = 1;
    m_ukui_SearchLine = ui->SearchLine;
    m_newKynote = ui->newKynote;
    m_trashButton = ui->add_more_btn;
    m_countLabel = ui->label;
    m_sortLabel = ui->sort_btn;

    m_trashButton->setToolTip(QStringLiteral("Delete Selected Note"));
    m_newKynote->setToolTip(QStringLiteral("Create New Note"));
//    //窗口属性
//    setWindowFlags(Qt::FramelessWindowHint);//开启窗口无边框
//    setWindowOpacity(0.8);//窗口透明度
//    //弹出位置
//    QDesktopWidget *desktop = QApplication::desktop();
//    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
//    //组件属性
//    //ui->listWidget->setAttribute(Qt::WA_TranslucentBackground);//设置透明度
//    //ui->newKynote->setAttribute(Qt::WA_TranslucentBackground);
//    //标题
//    this->setWindowTitle(tr("Note"));
//    setWindowIcon(QIcon(":/new/prefix1/SVG/kylin-notebook.svg"));
//    //按钮
//    ui->newKynote->setStyleSheet(tristateButton(QPushButton,:/new/prefix1/SVG/new-b));
//    ui->pushButton_Mini->setStyleSheet(tristateButton(QPushButton,:/new/prefix1/SVG/dark_theme/min));
//    //ui->pushButton_Exit->setStyleSheet(tristateButton(QPushButton,:/new/prefix1/SVG/dark_theme/close));
//    //搜索框
//    ui->SearchLine->setPlaceholderText(QStringLiteral("Search..."));


    /*********ListWidget init ************/
    //设置滚动条样式(使用.qss)
    //ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏垂直滚动条
    //ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏水平滚动条

//    ui->listWidget->setStyleSheet("QListWidget::Item:hover{background-color:rgb(0,0,0);}"
//                                    "QlistWidget::Item:selected{background-color:rgb(0,0,0);}"
//                                    "QListWidget::Item:selected:!active{active{background-color:red;}");
    //ui->listWidget->verticalScrollBar()->setStyleSheet("QScrollBar{width:1px;}");
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(bmp.rect(),6,6);
    setMask(bmp);

    //setWindowOpacity(0.9);
    setWindowFlags(Qt::FramelessWindowHint);
    ui->SearchLine->setPlaceholderText(tr("搜索内容或修改时间内容"));//设置详细输入框的提示信息

    set_table_list_page_attribute();
    //set_tablewidget();
    //set_listwidget();
    set_all_btn_attribute();

    ui->tableView->hide();
    listflag = 1;

}

void Widget::ukui_conn()
{
    qDebug() << "conn";
    //主界面退出按钮
    connect(ui->pushButton_Exit,SIGNAL(clicked()),this,SLOT(exitSlot()));
    //主界面最小化按钮
    connect(ui->pushButton_Mini,SIGNAL(clicked()),this,SLOT(miniSlot()));   
    //新建按钮
    connect(m_newKynote,&QPushButton::clicked, this, &Widget::newSlot);
    //删除按钮
    connect(m_trashButton, &QPushButton::clicked, this, &Widget::onTrashButtonClicked);
    //升/降序按钮
    connect(m_sortLabel,&QPushButton::clicked,this,&Widget::sortSlot);
    //搜索栏文本输入
    connect(m_ukui_SearchLine, &QLineEdit::textChanged, this, &Widget::onSearchEditTextChanged);
    //listview单击事件
    connect(m_noteView, &NoteView::pressed, this, &Widget::onNotePressed);
    //listview双击事件
    connect(m_noteView,&NoteView::doubleClicked,this,&Widget::listDoubleClickSlot);
    // noteView viewport pressed
    connect(m_noteView, &NoteView::viewportPressed, this, [this](){
        qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
        if(m_isTemp && m_proxyModel->rowCount() > 1){
            QModelIndex indexInProxy = m_proxyModel->index(1, 0);
            selectNote(indexInProxy);
        }else if(m_isTemp && m_proxyModel->rowCount() == 1){
            QModelIndex indexInProxy = m_proxyModel->index(0, 0);
            deleteNote(indexInProxy, false);
        }
    });
    //connect(ui->ukui_SearchLine,&QLineEdit::textChanged,this,&Widget::lineeditChangedSlot);
//    connect(ui->searchClearButton,&QPushButton::clicked,this,[=]{
//        ui->ukui_SearchLine->setText("");
//    });

    // auto save timer
    connect(m_autoSaveTimer, &QTimer::timeout, [this](){
        m_autoSaveTimer->stop();
        saveNoteToDB(m_currentSelectedNoteProxy);
    });
    //指定传递信号的方式Qt::BlockingQueuedConnection
    //这种类型类似于QueuedConnection，但是它只能应用于跨线程操作即发送者和接收者处于不同的线程中的情况
    //并且信号发送者线程会阻塞等待接收者的槽函数执行结束
    connect(this, &Widget::requestNotesList,
            m_dbManager,&DBManager::onNotesListRequested, Qt::BlockingQueuedConnection);
    connect(this, &Widget::requestCreateUpdateNote,
            m_dbManager, &DBManager::onCreateUpdateRequested, Qt::BlockingQueuedConnection);
    connect(this, &Widget::requestDeleteNote,
            m_dbManager, &DBManager::onDeleteNoteRequested);
//    connect(this, &Widget::requestRestoreNotes,
//            m_dbManager, &DBManager::onRestoreNotesRequested, Qt::BlockingQueuedConnection);
//    connect(this, &Widget::requestImportNotes,
//            m_dbManager, &DBManager::onImportNotesRequested, Qt::BlockingQueuedConnection);
//    connect(this, &Widget::requestExportNotes,
//            m_dbManager, &DBManager::onExportNotesRequested, Qt::BlockingQueuedConnection);
    connect(this, &Widget::requestMigrateNotes,
            m_dbManager, &DBManager::onMigrateNotesRequested, Qt::BlockingQueuedConnection);
    connect(this, &Widget::requestMigrateTrash,
            m_dbManager, &DBManager::onMigrateTrashRequested, Qt::BlockingQueuedConnection);
    connect(this, &Widget::requestForceLastRowIndexValue,
            m_dbManager, &DBManager::onForceLastRowIndexValueRequested, Qt::BlockingQueuedConnection);

    connect(m_dbManager, &DBManager::notesReceived, this, &Widget::loadNotes);
}

void Widget::checkMigration()
{
    QFileInfo fi(m_settingsDatabase->fileName());
    QDir dir(fi.absolutePath());

    QString oldNoteDBPath(dir.path() + QDir::separator() + "Notes.ini");
    if(QFile::exists(oldNoteDBPath))
        migrateNote(oldNoteDBPath);

    QString oldTrashDBPath(dir.path() + QDir::separator() + "Trash.ini");
    if(QFile::exists(oldTrashDBPath))
        //migrateTrash(oldTrashDBPath);

    emit requestForceLastRowIndexValue(m_noteCounter);
}

void Widget::migrateNote(QString notePath)
{
    QSettings notesIni(notePath, QSettings::IniFormat);
    QStringList dbKeys = notesIni.allKeys();

    m_noteCounter = notesIni.value(QStringLiteral("notesCounter"), "0").toInt();
    QList<NoteData *> noteList;

    auto it = dbKeys.begin();
    for(; it < dbKeys.end()-1; it += 3){
        QString noteName = it->split(QStringLiteral("/"))[0];
        int id = noteName.split(QStringLiteral("_"))[1].toInt();

        // sync db index with biggest notes id
        m_noteCounter = m_noteCounter < id ? id : m_noteCounter;

        NoteData* newNote = new NoteData();
        newNote->setId(id);

        QString createdDateDB = notesIni.value(noteName + QStringLiteral("/dateCreated"), "Error").toString();
        newNote->setCreationDateTime(QDateTime::fromString(createdDateDB, Qt::ISODate));
        QString lastEditedDateDB = notesIni.value(noteName + QStringLiteral("/dateEdited"), "Error").toString();
        newNote->setLastModificationDateTime(QDateTime::fromString(lastEditedDateDB, Qt::ISODate));
        QString contentText = notesIni.value(noteName + QStringLiteral("/content"), "Error").toString();
        newNote->setContent(contentText);
        QString firstLine = getFirstLine(contentText);
        newNote->setFullTitle(firstLine);

        noteList.append(newNote);
    }

    if(!noteList.isEmpty())
        emit requestMigrateNotes(noteList);

    QFile oldNoteDBFile(notePath);
    oldNoteDBFile.rename(QFileInfo(notePath).dir().path() + QDir::separator() + QStringLiteral("oldNotes.ini"));
}

void Widget::set_all_btn_attribute()
{
    pixmap1 = QPixmap(":/image/1x/新建-new.png)");
    pixmap2 = QPixmap(":/image/1x/window-close-symbolic.png");
    pixmap3 = QPixmap(":/image/1x/window-minimize-symbolic.png");
    pixmap4 = QPixmap(":/image/1x/open-menu-symbolic.png");
    pixmap5 = QPixmap(":/image/1x/图标模式.png");
    pixmap6 = QPixmap(":/image/1x/插入多选框-new.png");
    pixmap7 = QPixmap(":/image/1x/列表模式.png");
    pixmap8 = QPixmap(":/image/1x/排列-new.png");
    pixmap9 = QPixmap(":/image/1x/go-bottom-symbolic.png");


    ui->newKynote->setIcon(pixmap1);
    ui->pushButton_Exit->setIcon(pixmap2);
    ui->pushButton_Mini->setIcon(pixmap3);
    ui->set_btn->setIcon(pixmap4);
    ui->change_page_btn->setIcon(pixmap5);
    ui->add_more_btn->setIcon(pixmap6);
    ui->sort_btn->setIcon(pixmap8);
    ui->sort_2_btn->setIcon(pixmap9);
}

void Widget::set_table_list_page_attribute()
{

}

void Widget::set_tablewidget()
{

}

void Widget::deleteNote(const QModelIndex &noteIndex, bool isFromUser)
{
    qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
    if(noteIndex.isValid()){
        // delete from model
        QModelIndex indexToBeRemoved = m_proxyModel->mapToSource(m_currentSelectedNoteProxy);
        NoteData* noteTobeRemoved = m_noteModel->removeNote(indexToBeRemoved);

        if(m_isTemp){
            m_isTemp = false;
            --m_noteCounter;
        }else{
            noteTobeRemoved->setDeletionDateTime(QDateTime::currentDateTime());
            qDebug() << "emit requestDeleteNote";
            emit requestDeleteNote(noteTobeRemoved);
        }

        if(isFromUser){
            // clear text edit and time date label
//            m_textEdit->blockSignals(true);
//            m_textEdit->clear();
//            m_textEdit->clearFocus();
//            m_textEdit->blockSignals(false);

            if(m_noteModel->rowCount() > 0){
                QModelIndex index = m_noteView->currentIndex();
                m_currentSelectedNoteProxy = index;
            }else{
                m_currentSelectedNoteProxy = QModelIndex();
            }
        }
    }else{
        qDebug() << "Widget::deleteNote noteIndex is not valid";
    }

    m_noteView->setFocus();
}

void Widget::deleteSelectedNote()
{
    qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
    if(!m_isOperationRunning){
        m_isOperationRunning = true;
        if(m_currentSelectedNoteProxy.isValid()){

            // update the index of the selected note before searching
            if(!m_ukui_SearchLine->text().isEmpty()){
                qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
                QModelIndex currentIndexInSource = m_proxyModel->mapToSource(m_currentSelectedNoteProxy);
                int beforeSearchSelectedRow = m_selectedNoteBeforeSearchingInSource.row();
                if(currentIndexInSource.row() < beforeSearchSelectedRow){
                    m_selectedNoteBeforeSearchingInSource = m_noteModel->index(beforeSearchSelectedRow-1);
                }
            }
            qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
            qDebug() << m_currentSelectedNoteProxy;
            deleteNote(m_currentSelectedNoteProxy, true);
        }
        m_isOperationRunning = false;
    }
}

void Widget::onNotePressed(const QModelIndex& index)
{
    qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
    if(sender() != Q_NULLPTR){
        QModelIndex indexInProxy = m_proxyModel->index(index.row(), 0);
        if(indexInProxy.isValid()){
            // save the position of text edit scrollbar
            if(!m_isTemp && m_currentSelectedNoteProxy.isValid()){
                //int pos = m_textEdit->verticalScrollBar()->value();
                qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
                qDebug() << m_currentSelectedNoteProxy;
                QModelIndex indexSrc = m_proxyModel->mapToSource(m_currentSelectedNoteProxy);
                //m_noteModel->setData(indexSrc, QVariant::fromValue(pos), NoteModel::NoteScrollbarPos);
            }

            if(m_isTemp && indexInProxy.row() != 0){
                // delete the unmodified new note
                deleteNote(m_currentSelectedNoteProxy, false);
                m_currentSelectedNoteProxy = m_proxyModel->index(indexInProxy.row()-1, 0);
            }else if(!m_isTemp
                     && m_currentSelectedNoteProxy.isValid()
                     && indexInProxy != m_currentSelectedNoteProxy
                     && m_isContentModified){
                // save if the previous selected note was modified
                saveNoteToDB(m_currentSelectedNoteProxy);
                m_currentSelectedNoteProxy = indexInProxy;
            }else{
                m_currentSelectedNoteProxy = indexInProxy;
            }

            m_noteView->selectionModel()->select(m_currentSelectedNoteProxy, QItemSelectionModel::ClearAndSelect);
            m_noteView->setCurrentIndex(m_currentSelectedNoteProxy);
            m_noteView->scrollTo(m_currentSelectedNoteProxy);
        }else{
            qDebug() << "Widget::selectNote() : indexInProxy is not valid";
        }
        m_noteView->setCurrentRowActive(false);
    }
}

//双击选中笔记
void Widget::selectNote(const QModelIndex &noteIndex)
{
    qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
    qDebug() << noteIndex;
    if(noteIndex.isValid()){
        // save the position of text edit scrollbar
        if(!m_isTemp && m_currentSelectedNoteProxy.isValid()){
            //int pos = m_textEdit->verticalScrollBar()->value();
            QModelIndex indexSrc = m_proxyModel->mapToSource(m_currentSelectedNoteProxy);
            //m_noteModel->setData(indexSrc, QVariant::fromValue(pos), NoteModel::NoteScrollbarPos);
        }

        // show the content of the pressed note in the text editor
        showNoteInEditor(noteIndex);

        if(m_isTemp && noteIndex.row() != 0){
            // delete the unmodified new note
            deleteNote(m_currentSelectedNoteProxy, false);
            m_currentSelectedNoteProxy = m_proxyModel->index(noteIndex.row()-1, 0);
        }else if(!m_isTemp
                 && m_currentSelectedNoteProxy.isValid()
                 && noteIndex != m_currentSelectedNoteProxy
                 && m_isContentModified){
            // save if the previous selected note was modified
            saveNoteToDB(m_currentSelectedNoteProxy);
            m_currentSelectedNoteProxy = noteIndex;
        }else{
            m_currentSelectedNoteProxy = noteIndex;
        }

        m_noteView->selectionModel()->select(m_currentSelectedNoteProxy, QItemSelectionModel::ClearAndSelect);
        m_noteView->setCurrentIndex(m_currentSelectedNoteProxy);
        m_noteView->scrollTo(m_currentSelectedNoteProxy);
    }else{
        qDebug() << "Widget::selectNote() : noteIndex is not valid";
    }
}

void Widget::showNoteInEditor(const QModelIndex &noteIndex)
{
    qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
    //m_textEdit->blockSignals(true);

    /// fixing bug #202
    //m_textEdit->setTextBackgroundColor(QColor(255,255,255, 0));


    QString content = noteIndex.data(NoteModel::NoteContent).toString();
    QDateTime dateTime = noteIndex.data(NoteModel::NoteLastModificationDateTime).toDateTime();
    int scrollbarPos = noteIndex.data(NoteModel::NoteScrollbarPos).toInt();

    // set text and date
    m_notebook->ui->textEdit->setText(content);
    QString noteDate = dateTime.toString(Qt::ISODate);
    QString noteDateEditor = getNoteDateEditor(noteDate);
    // set scrollbar position
    //m_textEdit->verticalScrollBar()->setValue(scrollbarPos);
    //m_textEdit->blockSignals(false);

    //highlightSearch();
}

void Widget::selectFirstNote()
{
    if(m_proxyModel->rowCount() > 0){
        QModelIndex index = m_proxyModel->index(0,0);
        m_noteView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
        m_noteView->setCurrentIndex(index);

        m_currentSelectedNoteProxy = index;
    }
}

void Widget::createNewNoteIfEmpty()
{
    if(m_proxyModel->rowCount() == 0)
        createNewNote();
}

void Widget::loadNotes(QList<NoteData *> noteList, int noteCounter)
{
    //排序
    if(!noteList.isEmpty()){
        m_noteModel->addListNote(noteList);
        //Qt::AscendingOrder 升序排序
        //参见 NoteModel::sort
        m_noteModel->sort(0,Qt::AscendingOrder);
    }

    m_noteCounter = noteCounter;

    // TODO: move this from here
    createNewNoteIfEmpty();
    selectFirstNote();
    m_countLabel->setText(QObject::tr("%1 records in total").arg(m_proxyModel->rowCount()));
}

void Widget::moveNoteToTop()
{
    // check if the current note is note on the top of the list
    // if true move the note to the top
    if(m_currentSelectedNoteProxy.isValid()){

        m_noteView->scrollToTop();

        // move the current selected note to the top
        //当前要移动到顶端的item QSortFilterProxyModel
        QModelIndex sourceIndex = m_proxyModel->mapToSource(m_currentSelectedNoteProxy);

        //目前顶端的item QAbstractListModel
        QModelIndex destinationIndex = m_noteModel->index(0);

        //将 sourceIndex.row() 移动到第0行,第0行变第一行
        m_noteModel->moveRow(sourceIndex, sourceIndex.row(), destinationIndex, 0);

        // 更新当前 最顶端QAbstractListModel item 并添加代理
        m_currentSelectedNoteProxy = m_proxyModel->mapFromSource(destinationIndex);

        //修改当前选中
        m_noteView->setCurrentIndex(m_currentSelectedNoteProxy);
    }else{
        qDebug() << "Widget::moveNoteTop : m_currentSelectedNoteProxy not valid";
    }
}

QString Widget::getFirstLine(const QString& str)
{
    if(str.simplified().isEmpty())
        return "New Note";

    QString text = str.trimmed();
    QTextStream ts(&text);
    return ts.readLine(FIRST_LINE_MAX);
}

void Widget::findNotesContain(const QString& keyword)
{
    //将用于过滤源模型内容的固定字符串设置为给定模式
    m_proxyModel->setFilterFixedString(keyword);
    //m_clearButton->show();

    //如果匹配到不止一行
    if(m_proxyModel->rowCount() > 0){
        selectFirstNote();
    }else{
        m_currentSelectedNoteProxy = QModelIndex();
    }
}


void Widget::clearSearch()
{
    m_noteView->setFocusPolicy(Qt::StrongFocus);

    m_ukui_SearchLine->blockSignals(true);
    m_ukui_SearchLine->clear();
    m_ukui_SearchLine->blockSignals(false);
    m_proxyModel->setFilterFixedString(QString());

    //m_clearButton->hide();
    m_ukui_SearchLine->setFocus();
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    painter.setBrush(QBrush(QColor(14, 19, 22)));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 0);
    rect.setHeight(rect.height() - 0);
    painter.drawRoundedRect(rect, 7, 7);
    //也可用QPainterPath 绘制代替 painter.drawRoundedRect(rect, 15, 15);
    {
        QPainterPath painterPath;
        painterPath.addRoundedRect(rect, 7, 7);
        painter.drawPath(painterPath);
    }
    QWidget::paintEvent(event);
}

//********************Slots************************//

void Widget::onTextEditTextChanged()
{
    qDebug() << "receive signal textchange";
    if(m_currentSelectedNoteProxy.isValid()){
        m_notebook->ui->textEdit->blockSignals(true);
        QString content = m_currentSelectedNoteProxy.data(NoteModel::NoteContent).toString();
        if(m_notebook->ui->textEdit->toPlainText() != content){

            // move note to the top of the list
            QModelIndex sourceIndex = m_proxyModel->mapToSource(m_currentSelectedNoteProxy);
            qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
            qDebug() << m_currentSelectedNoteProxy.row(); //0
            bool b = ui->SearchLine->text().isEmpty();
            qDebug() << sourceIndex.row();  //0
            if(m_currentSelectedNoteProxy.row() != 0){
                moveNoteToTop();
            }else if(!ui->SearchLine->text().isEmpty() && sourceIndex.row() != 0){
                m_noteView->setAnimationEnabled(false);
                moveNoteToTop();
                m_noteView->setAnimationEnabled(true);
            }

            // Get the new data
            QString firstline = getFirstLine(m_notebook->ui->textEdit->toPlainText());
            QDateTime dateTime = QDateTime::currentDateTime();
            QString noteDate = dateTime.toString(Qt::ISODate);

            // update model
            QMap<int, QVariant> dataValue;
            dataValue[NoteModel::NoteContent] = QVariant::fromValue(m_notebook->ui->textEdit->toPlainText());
            dataValue[NoteModel::NoteFullTitle] = QVariant::fromValue(firstline);
            dataValue[NoteModel::NoteLastModificationDateTime] = QVariant::fromValue(dateTime);

            QModelIndex index = m_proxyModel->mapToSource(m_currentSelectedNoteProxy);
            m_noteModel->setItemData(index, dataValue);

            m_isContentModified = true;
            m_autoSaveTimer->start(500);
        }
        m_notebook->ui->textEdit->blockSignals(false);

        //获取当前调色板颜色

        //当前调色

        m_isTemp = false;
    }else{
        qDebug() << "Widget::onTextEditTextChanged() : m_currentSelectedNoteProxy is not valid";
    }
}

void Widget::onColorChanged(const QColor &color)
{
    qDebug() << "receive signal onColorChanged";
    if(m_currentSelectedNoteProxy.isValid()){
        const NoteWidgetDelegate delegate;
        int m_color = delegate.qcolorToInt(color);
        qDebug () << "m_color" << m_color;
        QMap<int, QVariant> dataValue;
        dataValue[NoteModel::NoteColor] = QVariant::fromValue(m_color);

        QModelIndex index = m_proxyModel->mapToSource(m_currentSelectedNoteProxy);
        m_noteModel->setItemData(index, dataValue);
        qDebug() << "m_currentSelectedNoteProxy" << m_currentSelectedNoteProxy.data(NoteModel::NoteColor).toInt();
        m_isColorModified = true;
        m_autoSaveTimer->start(500);
    }
}

void Widget::exitSlot(){
    tuichu = new tanchuang(this);
    tuichu->show();
    if(tuichu->close_flage)
    {
       this->close();
    }
}

void Widget::miniSlot()
{
    this->showMinimized();
}

void Widget::editSlot()
{
    qDebug() << "edit";
}
//将当前便笺保存到数据库
void Widget::saveNoteToDB(const QModelIndex& noteIndex)
{   //如果实例变量 noteIndex 是个有效对象 &&
    if(noteIndex.isValid() && m_isContentModified){
        //从排序过滤器模型返回与给定 noteIndex 对应的源模型索引。
        QModelIndex indexInSrc = m_proxyModel->mapToSource(noteIndex);
        NoteData* note = m_noteModel->getNote(indexInSrc);
        if(note != Q_NULLPTR)
            emit requestCreateUpdateNote(note);

        m_isContentModified = false;
    }else if(noteIndex.isValid() && m_isColorModified)
    {
        //从排序过滤器模型返回与给定 noteIndex 对应的源模型索引。
        QModelIndex indexInSrc = m_proxyModel->mapToSource(noteIndex);
        NoteData* note = m_noteModel->getNote(indexInSrc);
        if(note != Q_NULLPTR)
            emit requestCreateUpdateNote(note);
        m_isColorModified = false;
    }
}

QDateTime Widget::getQDateTime(QString date)
{
    QDateTime dateTime = QDateTime::fromString(date, Qt::ISODate);
    return dateTime;
}

QString Widget::getNoteDateEditor(QString dateEdited)
{
    QDateTime dateTimeEdited(getQDateTime(dateEdited));
    QLocale usLocale = QLocale::system();

    return usLocale.toString(dateTimeEdited, QStringLiteral("yyyy/MM/dd hh:mm"));
}
//初始化一个笔记
NoteData* Widget::generateNote(const int noteID)
{
    NoteData* newNote = new NoteData(this);
    newNote->setId(noteID);

    QDateTime noteDate = QDateTime::currentDateTime();
    newNote->setCreationDateTime(noteDate);
    newNote->setLastModificationDateTime(noteDate);
    newNote->setFullTitle(QStringLiteral("New Note"));

    return newNote;
}

/*
 * create a new note
 * add it to the database
 * add it to the scrollArea
 */
void Widget::createNewNote()
{
    if(!m_isOperationRunning){

        m_isOperationRunning = true;

        m_noteView->scrollToTop();

        // clear the textEdit
        //ukui_notebook->m_textEdit->blockSignals(true);
        //ukui_notebook->m_textEdit->clear();
        //ukui_notebook->m_textEdit->setFocus();
        //ukui_notebook->m_textEdit->blockSignals(false);

        if(!m_isTemp){
            qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
            ++m_noteCounter;
            NoteData* tmpNote = generateNote(m_noteCounter);
            m_isTemp = true;

            // insert the new note to NoteModel
            QModelIndex indexSrc = m_noteModel->insertNote(tmpNote, 0);

            // update the editor header date label
            QString dateTimeFromDB = tmpNote->lastModificationdateTime().toString(Qt::ISODate);
            QString dateTimeForEditor = getNoteDateEditor(dateTimeFromDB);

            // 从排序过滤器模型返回与给定 indexSrc 对应的源模型索引。
            m_currentSelectedNoteProxy = m_proxyModel->mapFromSource(indexSrc);

        }else{
            qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
            int row = m_currentSelectedNoteProxy.row();
            m_noteView->animateAddedRow(QModelIndex(),row, row);
        }
        //设置索引 m_currentSelectedNoteProxy 所在的页面为当前页面
        m_noteView->setCurrentIndex(m_currentSelectedNoteProxy);
        m_isOperationRunning = false;
    }
}

void Widget::newSlot()
{
    //新建一个笔记本
    m_notebook =  new Edit_page(this);
    m_notebook->show();
    m_notebook->ui->textEdit->setFocus();

    //如果搜索栏有内容,则在新建便签时清空
    if(!m_ukui_SearchLine->text().isEmpty())
    {
        clearSearch();
        m_selectedNoteBeforeSearchingInSource = QModelIndex();
    }

    // save the data of the previous selected
    if(m_currentSelectedNoteProxy.isValid()
            && m_isContentModified){
        qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
        qDebug() << m_currentSelectedNoteProxy;
        saveNoteToDB(m_currentSelectedNoteProxy);
        m_isContentModified = false;
    }

    this->createNewNote();
    m_countLabel->setText(QObject::tr("%1 records in total").arg(m_proxyModel->rowCount()));
    connect(m_notebook,SIGNAL(texthasChanged()), this,SLOT(onTextEditTextChanged()));
    connect(m_notebook,SIGNAL(colorhasChanged(QColor)),this,SLOT(onColorChanged(QColor)));
}

void Widget::onTrashButtonClicked()
{
    m_trashButton->blockSignals(true);
    deleteSelectedNote();
    m_trashButton->blockSignals(false);

    m_countLabel->setText(QObject::tr("%1 records in total").arg(m_proxyModel->rowCount()));
}

void Widget::listClickSlot()
{
//    int listnum = ui->listWidget->currentRow();
//    rowNum = sqlModel->rowCount();
//    for (int i = 0;i < rowNum; i++)
//    {
//        qDebug() << rowNum;
//        singleItem[i]->pushButtonDel->hide();
//    }
//    qDebug() << "listClickSLot  rowNum = " << listnum;
//    singleItem[listnum]->pushButtonDel->show();
}

//* 在滚动区域中单击便笺时：
//* 取消突出显示上一个选定的便笺
//* 如果在临时便笺存在时选择便笺，请删除临时便笺
//* 突出显示所选便笺
//* 将所选便笺内容加载到textedit
void Widget::listDoubleClickSlot(const QModelIndex& index)
{
    qDebug() << "listDoubleClickSlot(const QModelIndex& index)" << index;
    m_notebook =  new Edit_page(this);

    if(sender() != Q_NULLPTR){
        //获取当前选中item下标
        QModelIndex indexInProxy = m_proxyModel->index(index.row(), 0);
        selectNote(indexInProxy);
        m_noteView->setCurrentRowActive(false);
    }

    //设置鼠标焦点
    m_notebook->ui->textEdit->setFocus();
    //移动光标至行末
    m_notebook->ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);

    m_notebook->show();
    m_editors.push_back(m_notebook);

    connect(m_notebook,SIGNAL(texthasChanged()), this,SLOT(onTextEditTextChanged()));
    connect(m_notebook,SIGNAL(colorhasChanged(QColor)),this,SLOT(onColorChanged(QColor)));
}


void Widget::lineeditChangedSlot(const QString &text)
{
//    if (text != "") {
//        ui->searchClearButton->setVisible(true);
//    } else {
//        ui->searchClearButton->setVisible(false);
//    }
}

void Widget::onSearchEditTextChanged(const QString& keyword)
{
    m_searchQueue.enqueue(keyword);

    if(!m_isOperationRunning){
        m_isOperationRunning = true;
        if(m_isTemp){
            m_isTemp = false;
            --m_noteCounter;
            // prevent the line edit from emitting signal
            // while animation for deleting the new note is running
            m_ukui_SearchLine->blockSignals(true);
            m_currentSelectedNoteProxy = QModelIndex();
            QModelIndex index = m_noteModel->index(0);
            m_noteModel->removeNote(index);
            m_ukui_SearchLine->blockSignals(false);

            if(m_noteModel->rowCount() > 0){
                m_selectedNoteBeforeSearchingInSource = m_noteModel->index(0);
            }else{
                m_selectedNoteBeforeSearchingInSource = QModelIndex();
            }

        }else if(!m_selectedNoteBeforeSearchingInSource.isValid()
                 && m_currentSelectedNoteProxy.isValid()){

            m_selectedNoteBeforeSearchingInSource = m_proxyModel->mapToSource(m_currentSelectedNoteProxy);
        }

        if(m_currentSelectedNoteProxy.isValid()
                && m_isContentModified){

            saveNoteToDB(m_currentSelectedNoteProxy);
        }

        // disable animation while searching
        m_noteView->setAnimationEnabled(false);

        while(!m_searchQueue.isEmpty()){
            qApp->processEvents();
            QString str = m_searchQueue.dequeue();
            if(str.isEmpty()){
                m_noteView->setFocusPolicy(Qt::StrongFocus);
                clearSearch();
                QModelIndex indexInProxy = m_proxyModel->mapFromSource(m_selectedNoteBeforeSearchingInSource);
                selectNote(indexInProxy);
                m_selectedNoteBeforeSearchingInSource = QModelIndex();
            }else{
                m_noteView->setFocusPolicy(Qt::NoFocus);
                //过滤
                findNotesContain(str);
            }
        }

        m_noteView->setAnimationEnabled(true);
        m_isOperationRunning = false;
    }

    //highlightSearch();
}

void Widget::sortSlot()
{
    qDebug() << "当前文件 :" << __FILE__ << "当前函数 :" << __FUNCTION__ << "当前行号 :" << __LINE__;
    //排序
    //if(!noteList.isEmpty()){
        //m_noteModel->addListNote(noteList);
        //Qt::AscendingOrder 升序排序
        //参见 NoteModel::sort
    if(m_proxyModel->rowCount())
    {
        if(sortflag)
        {
            m_noteModel->sort(0,Qt::DescendingOrder);
            sortflag = 0;

        }else
        {
            m_noteModel->sort(0,Qt::AscendingOrder);
            sortflag = 1;
        }

    }
}

void Widget::on_sort_btn_clicked()
{
    if(dack_wight_flag)
    {

        //light_show();
        dack_wight_flag = 0;
    }else{


        //black_show();
        dack_wight_flag = 1;
    }

}

void Widget::black_show()
{
    this->setObjectName(QString::fromUtf8("便签本"));

    ui->widget->setStyleSheet("QWidget{background-color: rgba(19,20,20, 0.7);}");
    ui->frameRight->setStyleSheet(QString::fromUtf8("background:rgba(19,20,20,0);"));
    ui->verticalLayout_textEdit->setObjectName(QString::fromUtf8("verticalLayout_textEdit"));
    ui->newKynote->setStyleSheet(QString::fromUtf8("background:rgba(61,107,229,1);\n"
                                                 "color: rgb(255, 255, 255);"));
    ui->SearchLine->setStyleSheet(QString::fromUtf8("background-color: rgb(43,49,56);\n"
                                              "color: rgb(255, 255, 255);\n"
                                              "opacity:0.08;\n"
                                              "border-radius:4px;"));
//    ui->tableWidget->setStyleSheet(QString::fromUtf8("background-color: rgba(255, 255, 255, 0);\n"
//                                                     "selection-background-color:rgba(72,72,76,1);"));

    ui->listView->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255, 0);\n"
                                                "selection-background-color:rgba(72,72,76,1);"));
    ui->label->setStyleSheet(QString::fromUtf8("background-color: rgb();\n"
                                           "color: rgb(126, 126, 126);"));
    ui->sort_btn->setStyleSheet(QString::fromUtf8("background-color: rgba(19,20,20,0);"));
    ui->sort_2_btn->setStyleSheet(QString::fromUtf8("background-color: rgba(19,20,20,0);"));
}

void Widget::light_show()
{

    this->setObjectName(QString::fromUtf8("便签本"));

    ui->widget->setStyleSheet("QWidget{background-color: rgba(255, 255, 255, 0.3);}");

    ui->frameRight->setStyleSheet(QString::fromUtf8("background:rgba(233,233,233,0);"));
    ui->verticalLayout_textEdit->setObjectName(QString::fromUtf8("verticalLayout_textEdit"));
    ui->newKynote->setStyleSheet(QString::fromUtf8("background:rgba(61,107,229,1);\n"
                                                 "color: rgb(255, 255, 255);"));
    ui->SearchLine->setStyleSheet(QString::fromUtf8("background-color: rgba(198, 198, 198,0.9);\n"
                                              "color: rgb(0, 0, 0);\n"
                                              "opacity:0.08;\n"
                                              "border-radius:4px;"));
//    ui->tableWidget->setStyleSheet(QString::fromUtf8("background-color: rgba(255, 255, 255, 0);\n"
//                                                 "selection-background-color:rgba(255, 255, 255, 0);"));

    ui->listView->setStyleSheet(QString::fromUtf8("background-color: rgba(255, 255, 255, 0);\n"
                                               "selection-background-color:rgba(255, 255, 255, 0);"));

    ui->label->setStyleSheet(QString::fromUtf8("background-color: rgba();\n"
                                            "color: rgb(43,49,56);\n"
                                           "color: rgb(126, 126, 126);"));
    ui->sort_btn->setStyleSheet(QString::fromUtf8("background-color: rgba(233,233,233,0);"));
    ui->sort_2_btn->setStyleSheet(QString::fromUtf8("background-color: rgba(233,233,233,0);"));

}
