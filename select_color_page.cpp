#include "select_color_page.h"
#include "ui_select_color_page.h"
#include <QPainter>
#define SHADOW_WIDTH 0

select_color_page::select_color_page(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::select_color_page)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
}

select_color_page::~select_color_page()
{
    delete ui;
}



void select_color_page::paintEvent(QPaintEvent *)
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
