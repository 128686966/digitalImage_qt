#include "myimage.h"
#include <QDebug>
#include  <utility>
using namespace std;

MyImage::MyImage()
{

}

MyImage::MyImage(int width, int height, QImage::Format format):
    QImage(width,height,format)
{

}

Byte* MyImage::get_p(int x, int y)
{
    if(!(x >= 0 && y >= 0 && x < height() && y < width()) )
    {
        qDebug()<<"x|"<<x<<"y|"<<y;
    }
    Byte* pImageBytes = bits();//图像数据首地址
    int lineBytes = this->bytesPerLine();   //每行字节数
    int d = depth();
    return pImageBytes + x * lineBytes + y * (d/8); //注意不同位深偏移字节数不同
}

MyImage MyImage::get_spectrum(double coef)
{
    readImageData();
    DFT();
    return writeImage(coef);
}

MyImage MyImage::ifourier(double v)
{
    //本身不会得到结果, 只是做实验用
    readImageData();
    centralize();
    iDFT();
    return writeImage(v);
}

Byte MyImage::bi_interpol(int x1, int y1, int x2, int y2, double x, double y, int d)
{
    Byte g1 = get_p(x1, y1)[d];
    Byte g2 = get_p(x1, y2)[d];
    Byte g3 = get_p(x2, y1)[d];
    Byte g4 = get_p(x2, y2)[d];
    double gt1, gt2;
    Byte g;
    if(y2 == y1){
        gt1 = g1; gt2 = g3;
    }
    else{
        gt1 = g1 + double((g2 - g1))/(y2 - y1)*(y - y1);
        gt2 = g3 + double((g4 - g3))/(y2 - y1)*(y - y1);
    }
    if(x1 == x2){
        g = static_cast<Byte>(gt1);
    }
    else{
        g = static_cast<Byte>(gt1 + (gt2 - gt1)/(x2 - x1)*(x - x1));
    }
    return g;
}

void MyImage::DFT()
{
    centralize();  //这里自己选择是否进行中心化
    int wp = static_cast<int>(log(m_dataWidth)/log(2));   //m_dataWidth一定是2的整数幂
    for(int i = 0; i<m_dataHeight; i++)
        fft(&data[m_dataWidth*i], wp);//按行做傅里叶变换

    complex<double>* column = new complex<double>[m_dataHeight];
    int hp = static_cast<int>(log(m_dataHeight)/log(2));
    for(int i = 0; i < m_dataWidth; ++i)
    {
        for(int j = 0; j < m_dataHeight; ++j)
            column[j] = data[j * m_dataWidth + i];

        fft(column, hp);

        for(int j = 0; j < m_dataHeight; ++j)
            data[j * m_dataWidth + i] = column[j];
    }
    delete [] column;
}

void MyImage::iDFT()
{
    complex<double>* column = new complex<double>[m_dataHeight];
    int hp = static_cast<int>(log(m_dataHeight)/log(2));
    //----ifft----
    for(int i = 0; i < m_dataWidth; ++i)
    {
        for(int j = 0; j < m_dataHeight; ++j)
            column[j] = data[j * m_dataWidth + i];

        ifft(column, hp);

        for(int j = 0; j < m_dataHeight; ++j)
            data[j * m_dataWidth + i] = column[j];
    }
    delete [] column;

    int wp = static_cast<int>(log(m_dataWidth)/log(2));   //m_dataWidth一定是2的整数幂
    for(int i = 0; i<m_dataHeight; i++)
        ifft(&data[m_dataWidth*i], wp);//按行做傅里叶变换

    centralize();
}

MyImage MyImage::fft_ifft()
{
    readImageData();
    DFT();
    iDFT();
    return writeImage();
}

MyImage MyImage::histogram_equalization()
{
    int d = depth();
    int w = width();
    int h = height();

    Byte* pImageBytes = bits();    //图像数据首地址
    int lineBytes = this->bytesPerLine();   //每行字节数

    Byte* pByte;

    double histogram[256] = {0};  //todo: 这里应该可以调节显示颜色数量
    double N = w*h;
    double L = 256;

    //----获得直方图----
    int i, j;
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            if (8 == d) //采用了256色调色板，8位颜色索引
            {
                pByte = pImageBytes + i * lineBytes + j;
                histogram[*pByte] += 1/N;  //对应灰度数量增加(已经转化为频率)
            }

            else if(24==d)
            {
                pByte = pImageBytes + i * lineBytes + j * 3;
                //根据RGB模式转化成YIQ色彩模式的方式，取Y作为灰度值
                Byte pixelValue = static_cast<Byte>(
                          0.299 * static_cast<double>(pByte[0])
                        + 0.587 * static_cast<double>(pByte[1])
                        + 0.114 * static_cast<double>(pByte[2])
                        );
                histogram[pixelValue] += 1/N;  //对应灰度数量增加(已经转化为频率)
            }

            else if (32 == d)//32位表示，第一一字节存储透明度
            {
                pByte = pImageBytes + i * lineBytes + j * 4;
                Byte pixelValue = static_cast<Byte>(
                          0.299 * static_cast<double>(pByte[0])
                        + 0.587 * static_cast<double>(pByte[1])
                        + 0.114 * static_cast<double>(pByte[2])
                        );
                histogram[pixelValue] += 1/N;  //对应灰度数量增加(已经转化为频率)
            }
            else
            {
                qDebug("invalid format depth");
            }

        }
    }

    //----计算转换函数----
    for (int k = 1; k<L;k++){
        histogram[k] = histogram[k]+histogram[k-1]; //累加
    }
    Byte T[256] = {0};
    for (int k = 1; k<L;k++){
        T[k] = static_cast<Byte>(histogram[k]*(L-1));  //乘(L-1)然后四舍五入,得到转化函数
    }

    //----根据转换函数写入新图片----
    MyImage destImage(w, h, QImage::Format_Indexed8);
    int destLineBytes = destImage.bytesPerLine();
    Byte *pDestImageBytes = destImage.bits();
    Byte *pDestByte;
    for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                if (8 == d) //采用了256色调色板，8位颜色索引
                {

                    pByte = pImageBytes + i * lineBytes + j;
                    pDestByte = pDestImageBytes + i * destLineBytes + j;
                    *pDestByte = static_cast<Byte>(T[*pByte]);
                }

                else if(24==d)
                {
                    pByte = pImageBytes + i * lineBytes + j * 3;
                    //根据RGB模式转化成YIQ色彩模式的方式，取Y作为灰度值
                    Byte pixelValue = static_cast<Byte>(
                              0.299 * static_cast<double>(pByte[0])
                            + 0.587 * static_cast<double>(pByte[1])
                            + 0.114 * static_cast<double>(pByte[2])
                            );
                    pDestByte = pDestImageBytes + i * destLineBytes + j;
                    *pDestByte = static_cast<Byte>(T[pixelValue]);
                }

                else if (32 == d)//32位表示，第一一字节存储透明度
                {
                    pByte = pImageBytes + i * lineBytes + j * 4;
                    Byte pixelValue = static_cast<Byte>(
                              0.299 * static_cast<double>(pByte[0])
                            + 0.587 * static_cast<double>(pByte[1])
                            + 0.114 * static_cast<double>(pByte[2])
                            );
                    pDestByte = pDestImageBytes + i * destLineBytes + j;
                    *pDestByte = static_cast<Byte>(T[pixelValue]);
                }
                else
                {
                    qDebug("invalid format depth");
                }

            }
        }
    return destImage;
}

MyImage MyImage::rotation(double alpha, bool clip, int x0, int y0)
{
    // alpha: 旋转角度(角度制), x0, y0: 旋转中心, 默认为图像中心
    // 旋转后图像显示区域仍为矩形. 对于空余部分用0填充
    int w = this->width();
    int h = this->height();
    int d = this->depth();
    auto f = this->format();
    alpha = alpha / 180 * M_PI; //转换为弧度制
    double alpha_posi = abs(alpha);

    if(!clip){
        // 旋转后图像显示区域仍为矩形.
        int destW = static_cast<int>(w * cos(alpha_posi) + h * sin(alpha_posi));
        int destH = static_cast<int>(w * sin(alpha_posi) + h * cos(alpha_posi));
        MyImage destImage(destW, destH, f);
        for(int i = 0; i < destH; i++){
            for(int j = 0; j < destW; j++){
                double ii = i - 0.5 * destH + 0.5;
                double jj = j - 0.5 * destW + 0.5;
                double xx = ii * cos(alpha) + jj * sin(alpha);
                double yy = -ii * sin(alpha) + jj * cos(alpha);
                double x = xx - 0.5 + 0.5 * h;
                double y = yy - 0.5 + 0.5 * w;
                if (x < 0 || x >= h || y < 0 || y >= w){
                    for (int k = 0; k < d/8; k++)
                        destImage.get_p(i,j)[k] = 0; //对于空余部分用0填充
                }
                else{
                    int x1 = max(static_cast<int>(floor(x)), 0);
                    int y1 = max(static_cast<int>(floor(y)), 0);
                    int x2 = min(x1 + 1, h-1);
                    int y2 = min(y1 + 1, w-1);
                    // 对不同位深数值(如ARGB)运用双线性插值
                    for (int k = 0; k < d/8; k++)
                        destImage.get_p(i,j)[k] = this->bi_interpol(x1, y1, x2, y2, x, y, k);
                }
            }
        }
        return destImage;
    }
    else{
        MyImage destImage(w, h, f);
        for(int i = 0; i < h; i++){
            for(int j = 0; j < w; j++){
                double ii = i - x0 + 0.5;
                double jj = j - y0 + 0.5;
                double xx = ii * cos(alpha) + jj * sin(alpha);
                double yy = -ii * sin(alpha) + jj * cos(alpha);
                double x = xx - 0.5 + x0;
                double y = yy - 0.5 + y0;
                if (x < 0 || x >= h || y < 0 || y >= w){
                    for (int k = 0; k < d/8; k++)
                        destImage.get_p(i,j)[k] = 0; //对于空余部分用0填充
                }
                else{
                    int x1 = max(static_cast<int>(floor(x)), 0);
                    int y1 = max(static_cast<int>(floor(y)), 0);
                    int x2 = min(x1 + 1, h-1);
                    int y2 = min(y1 + 1, w-1);
                    // 对不同位深数值(如ARGB)运用双线性插值
                    for (int k = 0; k < d/8; k++)
                        destImage.get_p(i,j)[k] = this->bi_interpol(x1, y1, x2, y2, x, y, k);
                }
            }
        }
        return destImage;
    }
}

MyImage MyImage::scaling(double scale)
{
    assert(scale > 0);
    auto w = this->width();
    auto h = this->height();
    int destW = static_cast<int>(scale * w);
    int destH = static_cast<int>(scale * h); // 放大后图像的长宽映射到最近的整数
    QImage::Format f = this->format();
    MyImage destImage(destW, destH, f);

    for(int i = 0; i < destH; i++){
        for(int j = 0; j < destW; j++){
            //找到目标图像对应源图像中的位置, +-0.5相当于将缩放位置移动到图像中心
            double x = (i + 0.5) / scale - 0.5;
            double y = (j + 0.5) / scale - 0.5;
            //找到对应的四个最近的点
            int x1 = max(static_cast<int>(floor(x)), 0);
            int y1 = max(static_cast<int>(floor(y)), 0);
            int x2 = min(x1 + 1, h-1);
            int y2 = min(y1 + 1, w-1);
            // 对不同位深数值(如ARGB)运用双线性插值
            for (int k = 0; k < this->depth()/8; k++)
                destImage.get_p(i,j)[k] = this->bi_interpol(x1, y1, x2, y2, x, y, k);
        }
    }
    return destImage;
}

void MyImage::GHPF(double D0, double c, double A, double C)
{
    //H(u,v) = A[1-exp(-c*D^2(u,v)/(2*D0^2))]+C
    //D(u,v) = √((u-P/2)^2+(v-Q/2)^2)
    for(int u = 0; u < m_dataHeight;u++){
        for(int v = 0; v < m_dataWidth;v++){
            complex<double> D = sqrt((u-m_dataHeight*0.5)*(u-m_dataHeight*0.5)+(v-m_dataWidth*0.5)*(v-m_dataWidth*0.5));
            complex<double> p = -c*D*D*0.5*(1/D0)*(1/D0);
            complex<double> H = A*(1.0-exp(p))+C;
            data[u * m_dataWidth + v] = H * data[u * m_dataWidth + v];
        }
    }
}

MyImage MyImage::GHPF_hf(double gamma_H, double gamma_L, double c, double D0, double v)
{
    readImageData();
    DFT();
    GHPF(D0,c,(gamma_H-gamma_L),gamma_L);
    iDFT();
    return writeImage(v);
}

MyImage MyImage::homomorphic_filtering(double gamma_H, double gamma_L, double c, double D0, double v)
{
    readImageData();
    for(int u = 0; u < m_dataHeight;u++){
        for(int v = 0; v < m_dataWidth;v++){
            if(0.0 == data[u * m_dataWidth + v]) data[u * m_dataWidth + v] += 1e-10;//防止出现求零的对数的情况
            data[u * m_dataWidth + v] = log(data[u * m_dataWidth + v]);
        }
    }
    DFT();
    GHPF(D0,c,(gamma_H-gamma_L),gamma_L);
    iDFT();
    for(int u = 0; u < m_dataHeight;u++){
        for(int v = 0; v < m_dataWidth;v++){
            data[u * m_dataWidth + v] = exp(data[u * m_dataWidth + v]);
        }
    }
    return writeImage(v);
}

MyImage MyImage::adp_mean_filter(int S, int noiseVar)
{
    int w = this->width();
    int h = this->height();
    auto f = this->format();

    MyImage destImage(w, h, f);
    for(int i = 0; i<h;i++){
        for(int j = 0; j < w;j++){
            double mean = 0;
            double localVar = 0;
            int count = 0;
            for(int ii = i-S/2; ii <= i+S/2; ii++){
                for(int jj = j-S/2; jj <= j+S/2; jj++){
                    if(ii < 0 || ii >= h || jj < 0 ||jj>=w){
                        count ++;
                        continue;
                    }
                    double g_s = *get_p(ii,jj);
                    mean += g_s;
                    localVar += g_s*g_s;
                }
            }
            int S2 = S*S-count;
            mean = mean/S2;
            localVar = localVar/S2 - mean*mean;

            if(noiseVar > localVar)
                noiseVar = static_cast<int>(floor(localVar)); //防止出现noiseVar > localVar
            Byte g = *get_p(i,j);
            *destImage.get_p(i,j) = g - static_cast<Byte>(noiseVar*noiseVar/localVar/localVar*(g - mean));
        }
    }
    return destImage;
}

void MyImage::readImageData()
{
    int d = depth();
    int w = width();
    int h = height();

    //这里相当于取到大于等于w或h的 最小幂2数
    m_dataWidth = 1;
    m_dataHeight = 1;
    while(m_dataWidth < w) m_dataWidth *= 2;
    while(m_dataHeight < h) m_dataHeight *= 2;

    Byte* pImageBytes = bits();//图像数据首地址
    int lineBytes = this->bytesPerLine();   //每行字节数
    Byte* pByte;

    //遍历像素,将图像数据转存入data中
    int i, j;
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            if (8 == d) //采用了256色调色板，8位颜色索引
            {
                pByte = pImageBytes + i * lineBytes + j;
                data[i * m_dataWidth + j] = complex<double>(*pByte, 0);  //换成复数表示, 便于后续计算
                //此处使用m_dataWidth是为了扩展图片. data默认值为0
            }

            else if(24==d)
            {
                pByte = pImageBytes + i * lineBytes + j * 3;
                //根据RGB模式转化成YIQ色彩模式的方式，取Y作为灰度值
                Byte pixelValue = static_cast<Byte>(
                          0.299 * static_cast<double>(pByte[0])
                        + 0.587 * static_cast<double>(pByte[1])
                        + 0.114 * static_cast<double>(pByte[2])
                        );
                data[i * m_dataWidth + j] = complex<double>(pixelValue, 0);
            }

            else if (32 == d)//32位表示，第一一字节存储透明度
            {
                pByte = pImageBytes + i * lineBytes + j * 4;
                Byte pixelValue = rgb2gray(pByte[0],pByte[1],pByte[2]);  // todo qt有自带颜色类
                data[i * m_dataWidth + j] = complex<double>(pixelValue, 0);
            }
            else
            {
                qDebug("invalid format depth");
                return;
            }
        }
    }


}

MyImage MyImage::writeImage(double v)
{
    //v为图像灰度作对数映射的系数, 公式为 g' = log(1+v*g)/log(1+v);
    //v = 0 时不进行灰度变换

    MyImage destImage(m_dataWidth,m_dataHeight,QImage::Format_Indexed8);//将频谱图像也保存为8位灰度图像
    int w = destImage.width();
    int h = destImage.height();
    int lineBytes = destImage.bytesPerLine();
    Byte* pImageBytes = destImage.bits();
    Byte* pByte;
    double maxValue = 0;
    for (int i = 0; i< h; i++)
    {
        for (int j = 0; j< w; j++)
        {
            maxValue = max(maxValue, abs(data[i * w + j])); //计算最大值便于后续归一化
        }
    }

    for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                double spectral = abs(data[i * w + j]) / maxValue;  //归一化
                if(v > 0){
                    spectral = 255*log(1 + spectral * v)/log(v+1); //灰度值调整
                }
                else{
                    spectral = 255*spectral;
                }
                spectral = spectral > 255 ? 255 : spectral;
                //根据图像格式写数据
                pByte = pImageBytes + i * lineBytes + j;
                *pByte = static_cast<Byte>(spectral);
            }
        }
    return destImage;
}

void MyImage::centralize()
{
    for(int i = 0; i < m_dataHeight; ++i)
    {
        for(int j = 0; j < m_dataWidth; ++j)
        {
            data[i * m_dataWidth + j] = data[i * m_dataWidth + j]*pow(-1, i+j);
        }
    }
}

void fft(complex<double> a[], int power)
{
    // 对输入数组a进行快速傅里叶变换, 总信号数为2^power
    // 结果直接可以输出到a中, 节省空间

    if (power == 0) return;    //覆盖输出,不需要改变

    int n = 1 << power;     //总幂次数

    complex<double>* a0 = new complex<double>[n / 2]; //偶次项
    complex<double>* a1 = new complex<double>[n / 2]; //奇次项

    for (int i = 0; i < n / 2; i++)
    {
        a0[i] = a[2 * i];
        a1[i] = a[2 * i + 1];
    }//按奇偶次拆分

    //递归计算fft子过程
    fft(a0, power - 1);
    fft(a1, power - 1);

    //将变换后数组合并
    double angle = - 2 * M_PI / n;//按照书中的写法, 旋转因子W的系数应该为负
    complex<double> wn(cos(angle), sin(angle)); //主n次单位根
    complex<double> w(1, 0); //旋转因子(当前合并时奇次项前的系数)
    complex<double> u;
    for (int k = 0; k < n / 2; ++k)
    {
        u = w * a1[k];  //蝶形算法
        a[k] = a0[k] + u;
        a[k + n / 2] = a0[k] - u;
        w = w * wn;
    }

    delete[] a0;
    delete[] a1;
}

void ifft(complex<double> a[], int power) {
    // 对输入数组a进行快速傅里叶变换, 总信号数为2^power
    // 结果直接可以输出到a中, 节省空间

    if (power == 0) return;    //覆盖输出,不需要改变

    int n = 1 << power;     //总幂次数

    complex<double>* a0 = new complex<double>[n / 2]; //偶次项
    complex<double>* a1 = new complex<double>[n / 2]; //奇次项

    for (int i = 0; i < n / 2; i++)
    {
        a0[i] = a[2 * i];
        a1[i] = a[2 * i + 1];
    }//按奇偶次拆分

    //递归计算fft子过程
    ifft(a0, power - 1);
    ifft(a1, power - 1);

    //将变换后数组合并
    double angle = 2 * M_PI / n;
    complex<double> wn = complex<double>(cos(angle), sin(angle)); //主n次单位根
    complex<double> w(1, 0); //旋转因子(当前合并时奇次项前的系数)
    complex<double> u;
    for (int k = 0; k < n / 2; ++k)
    {
        u = w * a1[k];  //蝶形算法
        a[k] = (a0[k] + u) * 0.5;  //注意, 按公式里需要在最后的结果上乘以n, 用递归实现的话则需要在每一步结果上都乘0.5
        a[k + n / 2] = (a0[k] - u)*0.5;
        w = w * wn;
    }

    delete[] a0;
    delete[] a1;
}

Byte rgb2gray(Byte r, Byte g, Byte b)
{
    return static_cast<Byte>(0.299 * r + 0.587 * g + 0.114 * b);
}
