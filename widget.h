#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QListWidgetItem>
#include <QDesktopWidget>
#include <iostream>
#include <QTranslator>
#include <QStandardPaths>
#include <QStringLiteral>
#include "ui_ukui_notebook.h"
//#include "ui_singleitemwidget.h"
#include "mythrow.h"
#include <QPushButton>

#include "noteview.h"
#include "notedata.h"
#include "notemodel.h"
#include "dbmanager.h"
#include <QSettings>
#include <QThread>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QLabel>
#include <QProgressDialog>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QCheckBox>

#include <QBitmap>
#include <vector>
//#include "edit_page.h"
#include "tanchuang.h"
#include "ui_edit_page.h"

#define     tristateButton(className,imageUrl)     (""#className"{image:url("#imageUrl".svg);}   \
    "#className":hover{image:url("#imageUrl"-hover.svg);}  \
    "#className":pressed{image:url("#imageUrl"-click.svg);}")

namespace Ui {
class Widget;
}
class Edit_page;
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

public:
    std::vector<Edit_page*> m_editors;
    int dack_wight_flag;
    QTranslator *translator;                        //国际化
    void error_throw();                             //异常处理抛出

private:
    Ui::Widget *ui;                                 //主ui
    Edit_page *m_notebook;                   //新建便签指针
    std::vector<Edit_page*> m_Listnote;                   //便签列表笔记
    void set_table_list_page_attribute();
    void set_tablewidget();
    void set_listwidget();
    void black_show();
    void light_show();

    void set_all_btn_attribute();


    QPixmap pixmap1;
    QPixmap pixmap2;
    QPixmap pixmap3;
    QPixmap pixmap4;
    QPixmap pixmap5;
    QPixmap pixmap6;
    QPixmap pixmap7;
    QPixmap pixmap8;
    QPixmap pixmap9;
    QPixmap pixmap10;
    QPixmap pixmap11;
    int listflag;                                  //平铺/展开列表切换
    int sortflag;                                  //升降序切换
    tanchuang* tuichu;



    QTimer* m_autoSaveTimer;
    QSettings* m_settingsDatabase;
    QLineEdit* m_ukui_SearchLine;                   //搜索栏
    QPushButton* m_newKynote;                       //新建按钮
    QPushButton* m_trashButton;                     //删除按钮
    QLabel* m_countLabel;                           //item记数
    QPushButton* m_sortLabel;                       //升/降序按钮
    NoteView* m_noteView;
    NoteModel* m_noteModel;                         //
    NoteModel* m_deletedNotesModel;
    QSortFilterProxyModel* m_proxyModel;            //对项目进行排序，过滤
    QModelIndex m_currentSelectedNoteProxy;
    QModelIndex m_selectedNoteBeforeSearchingInSource;
    QQueue<QString> m_searchQueue;
    DBManager* m_dbManager;
    QThread* m_dbThread;

    int m_noteCounter;
    int m_trashCounter;
    bool m_isContentModified;                       //便签内容是否修改
    bool m_isColorModified;                         //便签颜色是否修改
    bool m_isTemp;
    bool m_isOperationRunning;



    void ukui_init();                               //加载界面组件
    void ukui_conn();                               //绑定槽函数
    void sqlInit();                                 //加载数据库
    void sqlAddItem();                              //插入数据库，同步插入item
    void sqlUpdateItem();                           //同步数据库，同步更新item



    //void InitData();
    void createNewNote();                           //新建笔记
    void deleteNote(const QModelIndex& noteIndex, bool isFromUser=true);
    void deleteSelectedNote();
    void setupDatabases();                          //配置数据库
    void initializeSettingsDatabase();
    void createNewNoteIfEmpty();
    void setupModelView();
    void saveNoteToDB(const QModelIndex& noteIndex);
    NoteData* generateNote(const int noteID);
    QString getFirstLine(const QString& str);
    QString getNoteDateEditor (QString dateEdited);
    QDateTime getQDateTime(QString date);
    void showNoteInEditor(const QModelIndex& noteIndex);
    void selectFirstNote();
    void moveNoteToTop();
    void clearSearch();
    void highlightSearch() const;
    void findNotesContain(const QString &keyword);
    void selectNote(const QModelIndex& noteIndex);

    void checkMigration();
    void migrateNote(QString notePath);



private slots:
    void InitData();
    void loadNotes(QList<NoteData *> noteList, int noteCounter);
    void onTextEditTextChanged();
    void onColorChanged(const QColor &color);
    void onTrashButtonClicked();
    void onNotePressed(const QModelIndex &index);
    void onSearchEditTextChanged(const QString& keyword);

    void exitSlot();                                //关闭按钮槽函数
    void miniSlot();                                //最小化按钮槽函数
    void editSlot();                                //编辑按钮槽函数
    void newSlot();                                 //新建按钮槽函数
    void listClickSlot();                           //item点击事件槽函数
    void listDoubleClickSlot(const QModelIndex &);  //item双击事件槽函数
    void lineeditChangedSlot(const QString &text);  //搜索栏清空按钮显示
    void sortSlot();      //升/降序槽函数

    //void loadNotes(QList<NoteData *> noteList, int noteCounter);

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void requestNotesList();
    void requestOpenDBManager(QString path, bool doCreate);
    void requestCreateUpdateNote(NoteData* note);
    void requestDeleteNote(NoteData* note);
    void requestRestoreNotes(QList<NoteData *> noteList);
    void requestImportNotes(QList<NoteData *> noteList);
    void requestExportNotes(QString fileName);
    void requestMigrateNotes(QList<NoteData *> noteList);
    void requestMigrateTrash(QList<NoteData *> noteList);
    void requestForceLastRowIndexValue(int index);
};

#endif // WIDGET_H
