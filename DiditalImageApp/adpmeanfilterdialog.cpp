#include "adpmeanfilterdialog.h"
#include "ui_adpmeanfilterdialog.h"

AdpMeanFilterDialog::AdpMeanFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdpMeanFilterDialog)
{
    ui->setupUi(this);
}

AdpMeanFilterDialog::~AdpMeanFilterDialog()
{
    delete ui;
}

void AdpMeanFilterDialog::on_pushButton_clicked()
{
    int S =ui->S_spinBox->value();
    int noiseVar = ui->noiseVar_spinBox_2->value();

    emit sendVar_af(S,noiseVar);
}
