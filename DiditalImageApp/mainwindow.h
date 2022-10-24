#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include "myimage.h"
#include "rotationinputdialog.h"
#include "imagewidget.h"
#include "homofilterdialog.h"
#include "adpmeanfilterdialog.h"
#include "rawimage.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    const imgPtr get_Image_input(){ return m_imageInput; }
    const shared_ptr<RawImage> get_rawImage() {return m_rawImg; }

private slots:

    void on_pushButton_openImage_clicked();

    void on_pushButton_Run_clicked();

    void on_pushButton_clicked();

    void receive_var_rotate(double alpha, bool clip, int x0, int y0);

    void receive_var_hf(double gamma_H, double gamma_L, double c, double D0, double v, bool decomposed);

    void receive_var_af(int S, int noiseVar);

    void receive_var_lw(int level, int width);

    void show_input_image();
    void show_output_image();
protected:
    Ui::MainWindow *ui;
    imgPtr m_imageInput;
    imgPtr m_imageOutput;
    shared_ptr<RawImage> m_rawImg;
    ImageWidget *m_widgetInput;
    ImageWidget *m_widgetOutput;


};

#endif // MAINWINDOW_H
