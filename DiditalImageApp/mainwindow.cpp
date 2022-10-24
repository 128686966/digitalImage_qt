#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setlevwindialog.h"
#include<QtDebug>
#include<QInputDialog>
#include<QFormLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_imageInput(new MyImage()),
    m_imageOutput(new MyImage())
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_widgetInput;
    delete m_widgetOutput;
}

void MainWindow::on_pushButton_openImage_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"Choose Image", "/home","Images (*.png *.bmp *.jpg *.tif *.raw)");
    if(filename.split('.')[1] == "raw"){
        m_rawImg = make_shared<RawImage>(filename);
        m_imageInput = m_rawImg->raw2gry();

        setLevWinDialog *D = new setLevWinDialog(this);
        connect(D,
                SIGNAL(send_var_lw(int,int)),
                this,
                SLOT(receive_var_lw(int, int)));
        D->show();
    }
    else{
        m_imageInput->load(filename);
    }
    qDebug()<<m_imageInput->format();
    show_input_image();
}

void MainWindow::on_pushButton_Run_clicked()
{
    int index = ui->comboBox->currentIndex();

    switch (index) {
    case 0:{
        double v = QInputDialog::getDouble(this,"get Value v", "v",20000);
        *m_imageOutput = m_imageInput->get_spectrum(v);
    }
        break;
    case 1:{
        double v = QInputDialog::getDouble(this,"get Value v", "v",20000);
        *m_imageOutput = m_imageInput->ifourier(v);
    }
        break;
    case 2:

        *m_imageOutput = m_imageInput->fft_ifft();
        break;
    case 3:
        *m_imageOutput = m_imageInput->histogram_equalization();
        break;
    case 4:{
        double scale = QInputDialog::getDouble(this,"get scale", "scale",1,0,50,3);
        *m_imageOutput = m_imageInput->scaling(scale);
        break;
    }
    case 5:{
        RotationInputDialog *D = new RotationInputDialog(this);
        connect(D, SIGNAL(sendVariable_rot(double,bool,int,int)),this, SLOT(receive_var_rotate(double,bool,int,int)));
        D->exec();
        break;
    }
    case 6:{
        HomoFilterDialog *D = new HomoFilterDialog(this);
        connect(D,
                SIGNAL(sendVariable_hf(double,double,double,double,double,bool)),
                this,
                SLOT(receive_var_hf(double,double,double,double,double,bool))
                );
        D->show();
        break;
    }
    case 7:{
        AdpMeanFilterDialog *D = new AdpMeanFilterDialog(this);
        connect(D,
                SIGNAL(sendVar_af(int, int)),
                this,
                SLOT(receive_var_af(int,int))
                );
        D->show();
        break;
    }
    default:
        qDebug("wrong input");
    }
    show_output_image();
}

void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Image","","Images (*.png *.bmp *.jpg)");
    m_imageOutput->save(filename);
}

void MainWindow::receive_var_rotate(double alpha, bool clip, int x0, int y0)
{
    *m_imageOutput = m_imageInput->rotation(alpha, clip, x0, y0);
}

void MainWindow::receive_var_hf(double gamma_H, double gamma_L, double c, double D0, double v, bool decomposed)
{
    if(decomposed){
        *m_imageOutput = m_imageInput->homomorphic_filtering(gamma_H, gamma_L, c, D0, v);
    }
    else{
        *m_imageOutput = m_imageInput->GHPF_hf(gamma_H, gamma_L, c, D0, v);
    }
    show_output_image();
}

void MainWindow::receive_var_af(int S, int noiseVar)
{
    *m_imageOutput = m_imageInput->adp_mean_filter(S, noiseVar);
    show_output_image();
}

void MainWindow::receive_var_lw(int level, int width)
{
    if(m_rawImg){
        m_rawImg->setWinLevel(level);
        m_rawImg->setWinWidth(width);
        m_imageInput = m_rawImg->raw2gry();
        show_input_image();
        show_output_image();
    }
}

void MainWindow::show_input_image()
{
    QPixmap pixmap(QPixmap::fromImage(*m_imageInput));
    m_widgetInput = new ImageWidget(&pixmap, ui->graphicsView_input);

    qDebug()<<m_imageInput->format();
    ui->label_width_input->setText(QString::number(m_imageInput->width()));
    ui->label_height_input->setText(QString::number(m_imageInput->height()));
    ui->label_depth_input->setText(QString::number(m_imageInput->depth()));
}

void MainWindow::show_output_image()
{
    QPixmap pixmap(QPixmap::fromImage(*m_imageOutput));
    m_widgetOutput = new ImageWidget(&pixmap, ui->graphicsView_output);

    ui->label_width_output->setText(QString::number(m_imageOutput->width()));
    ui->label_height_output->setText(QString::number(m_imageOutput->height()));
    ui->label_depth_output->setText(QString::number(m_imageOutput->depth()));
}
