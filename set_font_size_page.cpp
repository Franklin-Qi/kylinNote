#include "set_font_size_page.h"
#include "ui_set_font_size_page.h"
#include "form.h"
#include "ui_form.h"

Set_font_size_page::Set_font_size_page(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Set_font_size_page)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    //setAttribute(Qt::WA_TranslucentBackground);
    set_listwidget();
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

Set_font_size_page::~Set_font_size_page()
{
    delete ui;
}


void Set_font_size_page::set_listwidget()
{
    for(int i=22; i<35 ; i++ )
    {
        int n = i-22;
        list_aItem[n] =new QListWidgetItem;
        list_aItem[n]->setSizeHint(QSize(30,27));

        ui->listWidget->addItem(list_aItem[n]);

        list_page[n] = new  Form();
        list_page[n]->resize(QSize(30,23));

        ui->listWidget->setItemWidget(list_aItem[n],list_page[n]);
        list_page[n]->ui->label->setText(QString::number(i));
    }
}
