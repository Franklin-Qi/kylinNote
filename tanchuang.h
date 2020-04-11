#ifndef TANCHUANG_H
#define TANCHUANG_H

#include <QDialog>
#include <QPainter>

class Widget;
namespace Ui {
class tanchuang;
}
///class Widget;
class tanchuang : public QDialog
{
    Q_OBJECT

public:
    explicit tanchuang(Widget* page,QWidget *parent=nullptr);
    ~tanchuang();

    int close_flage;
    Widget * p;


private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::tanchuang *ui;


};

#endif // TANCHUANG_H
