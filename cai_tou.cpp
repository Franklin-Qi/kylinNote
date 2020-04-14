#include "cai_tou.h"
#include "ui_cai_tou.h"

cai_tou::cai_tou(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cai_tou)
{
    ui->setupUi(this);
    this->resize(250,8);
}

cai_tou::~cai_tou()
{
    delete ui;
}
