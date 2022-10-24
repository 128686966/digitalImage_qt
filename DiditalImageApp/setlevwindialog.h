#ifndef SETLEVWINDIALOG_H
#define SETLEVWINDIALOG_H

#include <QDialog>

namespace Ui {
class setLevWinDialog;
}

class setLevWinDialog : public QDialog
{
    Q_OBJECT
signals:
    void send_var_lw(int level, int width);
public:
    explicit setLevWinDialog(QWidget *parent = nullptr);
    ~setLevWinDialog();

private slots:
    void on_levelSlider_valueChanged(int value);

    void on_widthSlider_valueChanged(int value);

    void on_levelBox_valueChanged(int arg1);

    void sendVar();

private:
    Ui::setLevWinDialog *ui;
};

#endif // SETLEVWINDIALOG_H
