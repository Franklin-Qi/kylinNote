#ifndef TEXT_EDITING_H
#define TEXT_EDITING_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QTextCharFormat>

class Set_font_size_page;
class Set_font_color_page;
class Widget;
namespace Ui {
class Text_editing;
}

class Text_editing : public QWidget
{
    Q_OBJECT


public:
    explicit Text_editing(Widget* page,QWidget *parent = nullptr);
    ~Text_editing();

    Widget* pNotebook;

    Ui::Text_editing *ui;
    void paintEvent(QPaintEvent *);
    Set_font_size_page *set_size_page = nullptr ;
    Set_font_color_page *set_color_fort_page = nullptr ;
private slots:

    void color_clicked();
    void on_light_blue_btn_clicked();

    void on_blue_btn_2_clicked();

private:
    void set_btn_image();
    void light_show();
    void black_show();


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
    QTimer *timer;

};

#endif // TEXT_EDITING_H
