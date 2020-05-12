#include "noteHeadMenu.h"
#include "ui_noteHeadMenu.h"

noteHeadMenu::noteHeadMenu(QWidget *parent) :
    QWidget(parent)
  , color_widget(15,161,90)
  , ui(new Ui::noteHeadMenu)

{
    ui->setupUi(this);
    this->resize(250,20);
    buttonInit();
}

noteHeadMenu::~noteHeadMenu()
{
    delete ui;
}

void noteHeadMenu::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    painter.setBrush(QBrush(color_widget));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 0);
    rect.setHeight(rect.height() - 0);
    painter.drawRoundedRect(rect, 7, 0);
    //也可用QPainterPath 绘制代替 painter.drawRoundedRect(rect, 15, 15);
    {
        QPainterPath painterPath;
        painterPath.addRoundedRect(rect, 7, 0);
        painter.drawPath(painterPath);
    }
    QWidget::paintEvent(event);
}

void noteHeadMenu::buttonInit()
{
    QPixmap pixmap1;
    QPixmap pixmap2;
    pixmap1 = QPixmap(":/image/1x/close_block.png");
    pixmap2 = QPixmap(":/image/1x/more_block.png");
    ui->pushButtonExit->setIcon(pixmap1);
    ui->pushButtonExit->setIconSize(QSize(20,20));
    ui->pushButtonMenu->setIcon(pixmap2);
    ui->pushButtonMenu->setIconSize(QSize(20,20));
}
