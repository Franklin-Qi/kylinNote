#ifndef SET_FONT_SIZE_PAGE_H
#define SET_FONT_SIZE_PAGE_H

#include <QWidget>
#include <QListWidgetItem>
#include <QLabel>
#include "form.h"
#include "ui_form.h"

namespace Ui {
class Set_font_size_page;
}

class Set_font_size_page : public QWidget
{
    Q_OBJECT

public:
    explicit Set_font_size_page(QWidget *parent = nullptr);
    ~Set_font_size_page();
    void set_listwidget();
        Ui::Set_font_size_page *ui;


    QListWidgetItem *list_aItem[13];
    //QWidget *list_page[13];
    Form *list_page[13];
    QLabel *label[13];

private:



};

#endif // SET_FONT_SIZE_PAGE_H
