#ifndef NOTEHEADMENU_H
#define NOTEHEADMENU_H

#include <QWidget>
#include <QPainter>

namespace Ui {
class noteHeadMenu;
}

class noteHeadMenu : public QWidget
{
    Q_OBJECT

public:
    explicit noteHeadMenu(QWidget *parent = nullptr);
    ~noteHeadMenu();

    Ui::noteHeadMenu *ui;
    void paintEvent(QPaintEvent *event);
    QColor color_widget;

private:
    void buttonInit();
};

#endif // NOTEHEADMENU_H
