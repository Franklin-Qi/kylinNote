#include "ukui_notebook.h"
#include "ui_ukui_notebook.h"
#include "widget.h"

ukui_NoteBook::ukui_NoteBook(QWidget *parent, Widget *p) :
    QWidget(nullptr)
    , ui(new Ui::ukui_NoteBook)
    , m_textEdit(ui->textEdit)
{
    pParent = p;
    ui->setupUi(this);
    ukuiNoteBook_init();
    ukuiNoteBook_connect();
}

ukui_NoteBook::~ukui_NoteBook()
{
    if(notebook == nullptr){
        qDebug() <<"notebook == null";
    }
    else {
        notebook = nullptr;
        qDebug() << "notebook = null";
    }
    delete ui;
    qDebug() << "delete ui";
    delete ui;
}


void ukui_NoteBook::ukuiNoteBook_init()
{
    qDebug() << "ukuiNoteBook_init";
    //窗口属性
    setWindowFlags(Qt::FramelessWindowHint);//开启窗口无边框
    setWindowOpacity(0.8);//设置窗口透明度
    ui->textEdit->setWindowOpacity(0.9);
    //窗口弹出位置
    QDesktopWidget *desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
    this->setWindowTitle(tr("NoteBook"));
    //this->setStyleSheet("background-color: rgb(0,0,0)");
    //按钮属性
    ui->ukui_NoteClose->setFocusPolicy(Qt::NoFocus);
    //字体初始化
    setting = new QSettings("config.ini",QSettings::IniFormat);

}

void ukui_NoteBook::ukuiNoteBook_connect()
{
    qDebug() << "ukuiNoteBook_connect";
    connect(ui->textEdit,&QTextEdit::textChanged,this,&ukui_NoteBook::textChangedSlot);
    connect(ui->ukui_NoteNew,SIGNAL(clicked()),this,SLOT(newWindowsSlot()));
    connect(ui->ukui_NoteClose,SIGNAL(clicked()),this,SLOT(exitFileSlot()));
    connect(ui->ukui_NoteDate,SIGNAL(clicked()),this,SLOT(insertTimeDateSlot()));
    connect(ui->ukui_NotePdf,SIGNAL(clicked()),this,SLOT(exportPdfSlot()));
    connect(ui->ukui_NoteJpg,SIGNAL(clicked()),this,SLOT(exportJpgSlot()));
    qDebug() << "dadasd";
}



/***************Slots**************/

void ukui_NoteBook::textChangedSlot()
{
    qDebug() << "emit textchange";
    emit texthasChanged();
}

void ukui_NoteBook::exitFileSlot()
{
    this->close();
}

void ukui_NoteBook::insertTimeDateSlot()
{
    qDebug() << "..";
    QDateTime time = QDateTime::currentDateTime();//获取当前系统时间
    ui->textEdit->append(time.toString("hh:mm yyyy-MM-dd"));
}

void ukui_NoteBook::newWindowsSlot()
{
    qDebug() << "newWindowsSlot";
    notebook =  new ukui_NoteBook(nullptr, pParent);
    notebook->show();
}

void ukui_NoteBook::exportPdfSlot()
{
    qDebug() << "..";
    QString file = QFileDialog::getSaveFileName(this, "Export pdf", "", "Pdf file(*.pdf);;");
    if(!file.isEmpty()){
        if(QFileInfo(file).suffix().isEmpty()){
            file.append(".pdf");
        }
    }
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(file);
    ui->textEdit->document()->print(&printer);
}

void ukui_NoteBook::exportJpgSlot()
{
    qDebug() << "export jpg";
    int w,h;
    w = ui->textEdit->width();
    h = ui->textEdit->height();
    QPixmap exportImage(w,h);
    exportImage.fill(Qt::white);
    QPainter painter(&exportImage);
    QPixmap axisPixmap = ui->textEdit->grab(QRect(0,0,-1,-1));
    painter.drawPixmap(0,0,axisPixmap);

    QString pic = QFileDialog::getSaveFileName(this,"Export picture","","Images(*.jpg)");
    if(!pic.isEmpty()){
        if(QFileInfo(pic).suffix().isEmpty()){
            pic.append(".jpg");
        }
        exportImage.save(pic);
    }
}



