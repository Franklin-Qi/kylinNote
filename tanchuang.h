#ifndef TANCHUANG_H
#define TANCHUANG_H

#include <QDialog>
#include <QPainter>

class Widget;
namespace Ui {
class tanchuang;
}
///class Notebook;
class tanchuang : public QDialog
{
    Q_OBJECT

public:
    explicit tanchuang(Widget* page,QWidget *parent=nullptr);
    ~tanchuang();

    int close_flage;



private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void color_clicked();

private:
    Ui::tanchuang *ui;

    void light_show();
    void black_show();

    Widget* pNotebook;


};

#endif // TANCHUANG_H
