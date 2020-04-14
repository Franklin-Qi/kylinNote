#ifndef COLOR_MINI_H
#define COLOR_MINI_H

#include <QWidget>

namespace Ui {
class color_mini;
}

class color_mini : public QWidget
{
    Q_OBJECT

public:
    explicit color_mini(QWidget *parent = nullptr);
    ~color_mini();
    Ui::color_mini *ui;

private:

};

#endif // COLOR_MINI_H
