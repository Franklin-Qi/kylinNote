#ifndef EDIT_PAGE_H
#define EDIT_PAGE_H

#include <QWidget>
#include <QPainter>
#include <QMenu>
#include "select_color_page.h"
#include "text_editing.h"

namespace Ui {
class Edit_page;
}
class Widget;
class Edit_page : public QWidget
{
    Q_OBJECT

public:
    Edit_page(Widget* p , QWidget *parent = nullptr);
    ~Edit_page();
    Widget* pNotebook;
    void paintEvent(QPaintEvent *event)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
       // painter.setBrush(QBrush(QColor(14, 19, 22)));
        painter.setPen(Qt::transparent);
        QRect rect = this->rect();
        rect.setWidth(rect.width() - 0);
        rect.setHeight(rect.height() - 0);
        painter.drawRoundedRect(rect, 7, 7);
        //也可用QPainterPath 绘制代替 painter.drawRoundedRect(rect, 15, 15);
        {
            QPainterPath painterPath;
            painterPath.addRoundedRect(rect, 7, 7);
            painter.drawPath(painterPath);
        }
        QWidget::paintEvent(event);
    }
    Ui::Edit_page *ui;
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void textChangedSlot();

    void on_color_btn_clicked();
    void blue_btn_change();
    void pink_btn_change();
    void dark_green_btn_change();
    void orang_btn_change();
    void Violet_btn_change();
    void Golden_btn_change();
    void light_blue_btn_change();
    void light_green_btn_change();
    void yellow_btn_change();
    void wight_btn_change();
    void clear_the_page();
    void add_new_page();
    void show_note_page();


    void on_chang_btn_clicked();

private:
    void set_all_btn_attribute();
    void palette_ui();
    void set_select_color_page();
    void set_text_editing_page();




    QPixmap pixmap1;
    QPixmap pixmap2;
    QPixmap pixmap3;
    select_color_page *color_page ;
    Text_editing *text_edit_page;

signals:
    void texthasChanged();
};

#endif // EDIT_PAGE_H
