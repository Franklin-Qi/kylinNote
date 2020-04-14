#ifndef CAI_TOU_H
#define CAI_TOU_H

#include <QWidget>
#include <QPainter>

namespace Ui {
class cai_tou;
}

class cai_tou : public QWidget
{
    Q_OBJECT

public:
    explicit cai_tou(QWidget *parent = nullptr);
    ~cai_tou();


    void paintEvent(QPaintEvent *event)
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
    QColor color_widget =QColor(15,161,90) ;


private:
    Ui::cai_tou *ui;
};

#endif // CAI_TOU_H
