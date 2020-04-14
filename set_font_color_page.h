#ifndef SET_FONT_COLOR_PAGE_H
#define SET_FONT_COLOR_PAGE_H

#include <QWidget>
#include <QListWidgetItem>
#include "color_mini.h"
#include "ui_color_mini.h"
#include <QString>

namespace Ui {
class Set_font_color_page;
}

class Set_font_color_page : public QWidget
{
    Q_OBJECT

public:
    explicit Set_font_color_page(QWidget *parent = nullptr);
    ~Set_font_color_page();

    void set_listwidget();

    Ui::Set_font_color_page *ui;
    QListWidgetItem *list_aItem[10];
    color_mini *list_page[10];

    QString color[11];
    QString color1;

private:
    void set_color();

};

#endif // SET_FONT_COLOR_PAGE_H
