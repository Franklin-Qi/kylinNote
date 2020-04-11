#ifndef TEXT_EDITING_H
#define TEXT_EDITING_H

#include <QWidget>
#include <QPixmap>

namespace Ui {
class Text_editing;
}

class Text_editing : public QWidget
{
    Q_OBJECT

public:
    explicit Text_editing(QWidget *parent = nullptr);
    ~Text_editing();
        Ui::Text_editing *ui;
    void paintEvent(QPaintEvent *);
private:
    void set_btn_image();
    // 小三角起始位置;
    int m_startX = 220;
    // 小三角的宽度;
    int m_triangleWidth = 10;
    // 小三角高度;
    int m_triangleHeight= 5 ;

    QPixmap pixmap1;
    QPixmap pixmap2;
    QPixmap pixmap3;
    QPixmap pixmap4;
    QPixmap pixmap5;
    QPixmap pixmap6;
    QPixmap pixmap7;
    QPixmap pixmap8;

};

#endif // TEXT_EDITING_H
