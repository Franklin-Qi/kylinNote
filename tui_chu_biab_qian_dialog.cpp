#include "tui_chu_biab_qian_dialog.h"
#include "ui_tui_chu_biab_qian_dialog.h"
#include <QBitmap>
#include <QDebug>
#include <QPainter>

tui_chu_biab_qian_Dialog::tui_chu_biab_qian_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tui_chu_biab_qian_Dialog)
{
    ui->setupUi(this);

    ui->setupUi(this);
    //p = page;
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(bmp.rect(),6,6);
    setMask(bmp);

    //close_flage = 0;

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

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()) );
}

tui_chu_biab_qian_Dialog::~tui_chu_biab_qian_Dialog()
{
    delete ui;
}

void tui_chu_biab_qian_Dialog::on_pushButton_clicked()
{
    qDebug()<<"111";
    this->close();
}
