#include "tanchuang.h"
#include "ui_tanchuang.h"
#include <QBitmap>
#include <QPainter>
#include "widget.h"
tanchuang::tanchuang(Widget* page, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tanchuang)
{
    ui->setupUi(this);
    p = page;
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(bmp.rect(),6,6);
    setMask(bmp);

    close_flage = 0;

    this->setAttribute(Qt::WA_TranslucentBackground);
    ui->widget->setStyleSheet("QWidget{background-color: rgba(255, 255, 255, 0.4);}");
    ui->label->setStyleSheet("background-color: rgba();\
                             font-size:20px;\
                                font-weight:400;\
                                color:rgba(40,40,40,1);\
                                line-height:34px;\
                                opacity:0.97;\
                                 ");
     ui->checkBox->setStyleSheet("background-color: rgba();\
                                 font-size:14px;\
                                 font-weight:400;\
                                 color:rgba(102,102,102,1);\
                                 line-height:40px;\
                                 opacity:0.91;");

    setWindowFlags(Qt::FramelessWindowHint);

}

tanchuang::~tanchuang()
{
    delete ui;
}

void tanchuang::on_pushButton_clicked()
{
    this->close();
}

void tanchuang::on_pushButton_2_clicked()
{
    close_flage = 1;
    this->close();
    p->close();

}
