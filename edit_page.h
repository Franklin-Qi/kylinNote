#ifndef EDIT_PAGE_H
#define EDIT_PAGE_H

#include <QWidget>
#include <QPainter>
#include <QMenu>
#include "select_color_page.h"
#include "text_editing.h"
#include <QPainter>
#include <QColor>
#include "cai_tou.h"

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

    Ui::Edit_page *ui;
    Widget* pNotebook;
    QColor m_editColor;


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
    void color_clicked();
    void on_chang_btn_clicked();
    void showBoldBtn();
    void showItalicBtn();
    void showUnderlineBtn();
    void showStrikeOutResolved();
    //void showCurrentFormatChanged(const QTextCharFormat &);
    void showList(bool );
    void showNUMList(bool);
    void showSizeSpinBix();
    void showfortcolor();

private:
    void set_all_btn_attribute();
    void palette_ui();
    void set_select_color_page();
    void set_text_editing_page();
    void light_show();
    void black_show();
    void set_color();

    QPixmap pixmap1;
    QPixmap pixmap2;
    QPixmap pixmap3;
    select_color_page *color_page ;
    Text_editing *text_edit_page;
    QTimer *timer;
    QString color[11];
    QColor color_num[11];
    cai_tou *caitou;

signals:
    void texthasChanged();
    void colorhasChanged(const QColor &color);
};

#endif // EDIT_PAGE_H
