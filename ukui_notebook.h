#ifndef UKUI_NOTEBOOK_H
#define UKUI_NOTEBOOK_H

#include <QWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QFontDialog>
#include <QDateTime>
#include <QPainter>
#include <QPrinter>
#include <QMouseEvent>
#include <QTextEdit>
#include <QStyleOption>
#include <QDesktopWidget>
namespace Ui {
class ukui_NoteBook;
}
class Widget;
class ukui_NoteBook : public QWidget
{
    Q_OBJECT

public:
    explicit ukui_NoteBook(QWidget *parent = nullptr,Widget *p=nullptr);
    ~ukui_NoteBook();

signals:
    void listItemClicked(int);
    void fileSaved(QString);
    void texthasChanged();

public:
    Ui::ukui_NoteBook *ui;
    Widget *pParent;                        //主窗口指针
    QTextEdit* m_textEdit;


private slots:
    void textChangedSlot();
    void exitFileSlot();                    //退出软件
    void insertTimeDateSlot();              //插入时间/日期
    void newWindowsSlot();                  //新建窗口
    void exportPdfSlot();                   //导出pdf
    void exportJpgSlot();                   //导出jpg/png

private:
    ukui_NoteBook *notebook;
    QString fileContent;                    //读到的文件内容
    QSettings *setting;                     //字体设置
    void saveTextToFile();                  //保存文本到文件   未实现
    void ukuiNoteBook_init();               //初始化
    void ukuiNoteBook_connect();            //关联槽函数
};

#endif // UKUI_NOTEBOOK_H
