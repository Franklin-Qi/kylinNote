#include "text_editing.h"
#include "ui_text_editing.h"
#include <QPainter>
#include <QDebug>
#define SHADOW_WIDTH 0

Text_editing::Text_editing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Text_editing)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);

    set_btn_image();
}

Text_editing::~Text_editing()
{
    delete ui;
}

void Text_editing::set_btn_image()
{
 pixmap1=QPixmap(":/image/1x/加粗-new.png");
 pixmap2=QPixmap(":/image/1x/斜体-new.png");
 pixmap3=QPixmap(":/image/1x/下划线-new.png");
 pixmap4=QPixmap(":/image/1x/删除线-new.png");
 pixmap5=QPixmap(":/image/1x/项目符号-new.png");
 pixmap6=QPixmap(":/image/1x/编号 副本.png");
 pixmap7=QPixmap(":/image/1x/)");
 pixmap8=QPixmap(":/image/1x/)");


 ui->orang_btn->setIcon(pixmap1);
 ui->dark_green_btn->setIcon(pixmap2);
 ui->pink_btn->setIcon(pixmap3);
 ui->Violet_btn->setIcon(pixmap4);
 ui->Golden_btn->setIcon(pixmap5);
 ui->light_green_btn->setIcon(pixmap6);

 //ui->light_blue_btn->setStyleSheet("background:rgba(255,255,255,1);\
                                   ");
}


void Text_editing::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0));

    // 小三角区域;
    QPolygon trianglePolygon;
    trianglePolygon << QPoint(m_startX, m_triangleHeight + SHADOW_WIDTH);
    trianglePolygon << QPoint(m_startX + m_triangleWidth / 2, SHADOW_WIDTH);
    trianglePolygon << QPoint(m_startX + m_triangleWidth, m_triangleHeight + SHADOW_WIDTH);

    QPainterPath drawPath;

    drawPath.addPolygon(trianglePolygon);
    painter.drawPath(drawPath);
}
