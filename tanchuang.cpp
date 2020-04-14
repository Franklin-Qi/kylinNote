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
    pNotebook = page;
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
    setWindowFlags(Qt::FramelessWindowHint);
    color_clicked();
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
    pNotebook->close();
//    if(ui->checkBox->isChecked()==true)
//    {
//        for (auto it = pNotebook->m_editors.begin(); it!= pNotebook->m_editors.end();it++) {
//            (*it)->close();
//        }
//    }
}

void tanchuang::color_clicked()
{
    if(!pNotebook->dack_wight_flag == -1){
        return;
    }
        if(!pNotebook->dack_wight_flag)
        {
            light_show();
        }else{

            black_show();
        }
}

void tanchuang::light_show()
{
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
     ui->pushButton->setStyleSheet(QString::fromUtf8("color:rgba(102,102,102,1);\n"
                                                 "line-height:24px;"));
     ui->pushButton_2->setStyleSheet(QString::fromUtf8("color:rgba(102,102,102,1);\n"
                                                 "line-height:24px;"));
}

void tanchuang::black_show()
{
    ui->widget->setStyleSheet("QWidget{background-color: rgba(0, 0, 0, 0.4);}");

    ui->label->setStyleSheet("background-color: rgba();\
                             font-size:20px;\
                                font-weight:400;\
                                color:rgba(255,255,255,1);\
                                line-height:34px;\
                                opacity:0.97;\
                                 ");
     ui->checkBox->setStyleSheet("background-color: rgba();\
                                 font-size:14px;\
                                 font-weight:400;\
                                 color:rgba(255,255,255,1);\
                                 line-height:40px;\
                                 opacity:0.91;");
     ui->pushButton->setStyleSheet(QString::fromUtf8("color:rgba(255,255,255,1);\n"
                                                      "line-height:24px;"));
     ui->pushButton_2->setStyleSheet(QString::fromUtf8("color:rgba(255,255,255,1);\n"
                                                      "line-height:24px;"));
}
