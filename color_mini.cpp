#include "color_mini.h"
#include "ui_color_mini.h"

color_mini::color_mini(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::color_mini)
{
    ui->setupUi(this);
}

color_mini::~color_mini()
{
    delete ui;
}
