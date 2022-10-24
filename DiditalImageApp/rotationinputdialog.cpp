#include "rotationinputdialog.h"
#include "ui_rotationinputdialog.h"
#include "mainwindow.h"

RotationInputDialog::RotationInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RotationInputDialog)
{
    ui->setupUi(this);
    connect(ui->clipCheckBox,
            SIGNAL(stateChanged(int)),
            this,
            SLOT(onCheckedChange())
            );
    ui->x0Box->setValue(dynamic_cast<MainWindow*>(parent)->get_Image_input()->height()/2);
    ui->y0Box->setValue(dynamic_cast<MainWindow*>(parent)->get_Image_input()->width()/2);
}

RotationInputDialog::~RotationInputDialog()
{
    delete ui;
}

void RotationInputDialog::onCheckedChange()
{
    if (ui->clipCheckBox->isChecked()){
        ui->x0Box->setEnabled(true);
        ui->y0Box->setEnabled(true);
    }
    else{
        ui->x0Box->setEnabled(false);
        ui->y0Box->setEnabled(false);
    }
}

void RotationInputDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    double alpha = ui->RotationAngleBox->value();
    bool clip = ui->clipCheckBox->checkState();
    int x0 = ui->x0Box->value();
    int y0 = ui->y0Box->value();
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        //判断按下的是否为"确定”按钮
        emit sendVariable_rot(alpha, clip, x0, y0);
        QDialog::reject();
    }
    else if(button == ui->buttonBox->button(QDialogButtonBox::Cancel))
    {
        QDialog::reject();
    }
}
