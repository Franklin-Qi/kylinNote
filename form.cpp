#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    ui->label->setAlignment(Qt::AlignHCenter);
}

Form::~Form()
{
    delete ui;
}
