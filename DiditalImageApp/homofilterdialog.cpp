#include "homofilterdialog.h"
#include "ui_homofilterdialog.h"

HomoFilterDialog::HomoFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HomoFilterDialog)
{
    ui->setupUi(this);
    double gamma_H = ui->gammaH_value_slide->value()*0.01;
    double gamma_L = ui->gammaL_value_slide->value()*0.01;
    double c = ui->c_value_slide->value();
    double D0 = ui->D0_value_slide->value();
    double v = ui->v_value_slide->value();

    ui->gammaH_value_label->setNum(gamma_H);
    ui->gammaL_value_label->setNum(gamma_L);
    ui->c_value_label->setNum(static_cast<int>(c));
    ui->D0_value_label->setNum(static_cast<int>(D0));
    ui->v_value_label->setNum(static_cast<int>(v));

    bool decomposed = ui->CheckBox->isChecked();
    emit sendVariable_hf(gamma_H, gamma_L, c, D0, v, decomposed);
}

HomoFilterDialog::~HomoFilterDialog()
{
    delete ui;
}

void HomoFilterDialog::send_var_sig()
{
    double gamma_H = ui->gammaH_value_slide->value()*0.01;
    double gamma_L = ui->gammaL_value_slide->value()*0.01;
    double c = ui->c_value_slide->value();
    double D0 = ui->D0_value_slide->value();
    double v = ui->v_value_slide->value();

    ui->gammaH_value_label->setNum(gamma_H);
    ui->gammaL_value_label->setNum(gamma_L);
    ui->c_value_label->setNum(static_cast<int>(c));
    ui->D0_value_label->setNum(static_cast<int>(D0));
    ui->v_value_label->setNum(static_cast<int>(v));

    bool decomposed = ui->CheckBox->isChecked();
    emit sendVariable_hf(gamma_H, gamma_L, c, D0, v, decomposed);
}

void HomoFilterDialog::on_gammaH_value_slide_sliderReleased()
{
    send_var_sig();
}

void HomoFilterDialog::on_gammaL_value_slide_sliderReleased()
{
    send_var_sig();
}

void HomoFilterDialog::on_c_value_slide_sliderReleased()
{
    send_var_sig();
}

void HomoFilterDialog::on_D0_value_slide_sliderReleased()
{
    send_var_sig();
}

void HomoFilterDialog::on_CheckBox_stateChanged(int arg1)
{
    send_var_sig();
}

void HomoFilterDialog::on_v_value_slide_sliderReleased()
{
    send_var_sig();
}
