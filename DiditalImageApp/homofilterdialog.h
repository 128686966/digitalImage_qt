#ifndef HOMOFILTERDIALOG_H
#define HOMOFILTERDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QString>

namespace Ui {
class HomoFilterDialog;
}

class HomoFilterDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendVariable_hf(double gamma_H, double gamma_L, double c, double D0, double v, bool decomposed);

public:
    explicit HomoFilterDialog(QWidget *parent = nullptr);
    ~HomoFilterDialog();

private:
    Ui::HomoFilterDialog *ui;
    void send_var_sig();
private slots:
    void on_gammaH_value_slide_sliderReleased();
    void on_gammaL_value_slide_sliderReleased();
    void on_c_value_slide_sliderReleased();
    void on_D0_value_slide_sliderReleased();
    void on_CheckBox_stateChanged(int arg1);
    void on_v_value_slide_sliderReleased();
};

#endif // HOMOFILTERDIALOG_H
