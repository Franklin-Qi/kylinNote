#ifndef SELECT_COLOR_PAGE_H
#define SELECT_COLOR_PAGE_H

#include <QWidget>

#define SHADOW_WIDTH 15                 // 窗口阴影宽度;
#define TRIANGLE_WIDTH 15               // 小三角的宽度;
#define TRIANGLE_HEIGHT 10              // 小三角的高度;
#define BORDER_RADIUS 5                 // 窗口边角的弧度;

namespace Ui {
class select_color_page;
}

class select_color_page : public QWidget
{
    Q_OBJECT

public:
    explicit select_color_page(QWidget *parent = nullptr);
    ~select_color_page();
    void paintEvent(QPaintEvent *);
     Ui::select_color_page *ui;

private:


    // 小三角起始位置;
    int m_startX = 180;
    // 小三角的宽度;
    int m_triangleWidth = 10;
    // 小三角高度;
    int m_triangleHeight= 5 ;
};

#endif // SELECT_COLOR_PAGE_H
