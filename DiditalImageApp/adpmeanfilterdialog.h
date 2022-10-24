#ifndef ADPMEANFILTERDIALOG_H
#define ADPMEANFILTERDIALOG_H

#include <QDialog>

namespace Ui {
class AdpMeanFilterDialog;
}

class AdpMeanFilterDialog : public QDialog
{
    Q_OBJECT
signals:
    void sendVar_af(int S, int moiseVar);
public:
    explicit AdpMeanFilterDialog(QWidget *parent = nullptr);
    ~AdpMeanFilterDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::AdpMeanFilterDialog *ui;
};

#endif // ADPMEANFILTERDIALOG_H
