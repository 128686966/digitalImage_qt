#include "setlevwindialog.h"
#include"mainwindow.h"
#include "ui_setlevwindialog.h"

setLevWinDialog::setLevWinDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setLevWinDialog)
{
    ui->setupUi(this);

    int level = dynamic_cast<MainWindow*>(parent)->get_rawImage()->getWinLevel();
    int width = dynamic_cast<MainWindow*>(parent)->get_rawImage()->getWinWidth();
    ui->levelBox->setValue(level);
    ui->widthBox->setValue(width);
    ui->levelSlider->setValue(level);
    ui->widthSlider->setValue(level);


    connect(ui->levelBox, SIGNAL(valueChanged(int)), ui->levelSlider, SLOT(setValue(int)));
    connect(ui->levelSlider, SIGNAL(valueChanged(int)), ui->levelBox, SLOT(setValue(int)));

    connect(ui->widthBox, SIGNAL(valueChanged(int)), ui->widthSlider, SLOT(setValue(int)));
    connect(ui->widthSlider, SIGNAL(valueChanged(int)), ui->widthBox, SLOT(setValue(int)));
}

setLevWinDialog::~setLevWinDialog()
{
    delete ui;
}

void setLevWinDialog::on_levelSlider_valueChanged(int value)
{
    sendVar();
}

void setLevWinDialog::on_widthSlider_valueChanged(int value)
{
    sendVar();
}

void setLevWinDialog::on_levelBox_valueChanged(int arg1)
{

}

void setLevWinDialog::sendVar()
{
    int lev = ui->levelSlider->value();
    int wid = ui->widthSlider->value();
    emit send_var_lw(lev, wid);
}
