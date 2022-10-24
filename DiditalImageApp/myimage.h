#ifndef MYIMAGE_H
#define MYIMAGE_H

#define MaxSize 10000000  //最大像素总数

#include<QImage>
#include<complex>
#include<QMessageBox>
#include <cmath>
#include<vector>
#include<qfile.h>

typedef unsigned char Byte;

using namespace std;



class MyImage : public QImage
{
    //在QImage类下的衍生类, 用以实现多种转换功能
public:
    MyImage();
    MyImage(int width, int height, QImage::Format format);

    Byte* get_p(int x, int y); //返回指向x,y像素的指针

    //离散傅立叶变换, doCentr 为是否进行中心化操作, v为对数变换参数
    MyImage get_spectrum(double v=0);
    MyImage ifourier(double v=0);
    MyImage fft_ifft();  //对输入图片傅里叶变换后直接取反变换, 频谱结果不转换成图片
    MyImage histogram_equalization();  //直方图均衡化

    MyImage rotation(double alpha, bool clip = false, int x0 = 0, int y0 = 0);
    MyImage scaling(double scale);

    void GHPF(double D0, double c=1, double A=1, double C = 0); // H(u,v) = A(1-exp(-c * D^2(u,v)/(c2 * D_0^2)))+C
    MyImage GHPF_hf(double gamma_H, double gamma_L, double c, double D0, double v); //不进行分解的同态滤波(等同于略有改变的高斯滤波)
    MyImage homomorphic_filtering(double gamma_H, double gamma_L, double c, double D0, double v); //同态滤波(上面那个函数加上求对数和求导)
    MyImage adp_mean_filter(int S, int noiseVar);  //自适应均值滤波器

    MyImage load_raw(const QString &fileName);
private:
    int m_dataWidth;
    int m_dataHeight;

    //用data数组存放图像像素数组, 并初始化为(0,0) 使用复数是为了方便最后转化为频率
    //这里实际没有用(0,0)初始化, 因为complex本身构造函数缺省值就是(0,0), 自己实现反而更加麻烦
    complex<double> *data = new complex<double> [MaxSize];

    void readImageData();   //将图像数据移入全局数组data, 同时执行边长扩展
    MyImage writeImage(double v=0);    //将变换后结果写入 反变换时执行对数灰度变换
    void centralize();   //将data数据中心化 ( *(-1)^(x+y) )
    Byte bi_interpol(int x1, int y1, int x2, int y2, double x, double y, int d = 0);

    void DFT();
    void iDFT();
};


void fft(complex<double> a[], int power);
void ifft(complex<double> a[], int power);
Byte rgb2gray(Byte r, Byte g, Byte b);

#endif // MYIMAGE_H
