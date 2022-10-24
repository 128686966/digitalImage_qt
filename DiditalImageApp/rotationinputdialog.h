#ifndef ROTATIONINPUTDIALOG_H
#define ROTATIONINPUTDIALOG_H

#include <QPushButton>
#include <QDialog>

namespace Ui {
class RotationInputDialog;
}

class RotationInputDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendVariable_rot(double alpha, bool clip, int x0, int y0);

public:
    explicit RotationInputDialog(QWidget *parent = nullptr);
    ~RotationInputDialog();


private:
    Ui::RotationInputDialog *ui;

private slots:
    void onCheckedChange();
    void on_buttonBox_clicked(QAbstractButton *button);
};

#endif // ROTATIONINPUTDIALOG_H
