#ifndef TUI_CHU_BIAB_QIAN_DIALOG_H
#define TUI_CHU_BIAB_QIAN_DIALOG_H

#include <QDialog>

namespace Ui {
class tui_chu_biab_qian_Dialog;
}

class tui_chu_biab_qian_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit tui_chu_biab_qian_Dialog(QWidget *parent = nullptr);
    ~tui_chu_biab_qian_Dialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::tui_chu_biab_qian_Dialog *ui;
};

#endif // TUI_CHU_BIAB_QIAN_DIALOG_H
