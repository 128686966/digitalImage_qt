# 数字图像处理实验报告

61520216 石杰

## 总体框架

本程序由Qt实现, 主要功能由`MainWindow`, `MyImage`, `ImageWidget`三个类实现

- `MianWindow`类与Qt窗口绑定, 负责内容显示与用户交互; 
- `MyImage`类继承自`QImage`, 可以存储图像并实现多种图像变换功能, 如Fourier变换, 直方图均衡化等, 后续图像转换功能主要在该类基础上实现;
-  `ImageWidget`类负责将`MyImage`中的类显示到主窗口的`graphicView`上, 并实现拖拽, 缩放等功能

应用窗口效果如下图所示

![image-20220927202146957](.\assets\image-20220927202146957.png)

各交互元素功能如下

- Open Image: 打开文件夹窗口, 选择图片打开
- Option 选框: 选择要对图像进行的变换, 当前支持功能有
  - Fourier Transform: 傅里叶变换
  - Inverse Fourier Transform: 傅里叶逆变换
  - FFT -> IFFT 傅里叶变换后直接逆变换
  - Histogram Equalization: 直方图均衡化
- Run: 运行Option中选择的变换操作, 有输入需求时回跳出输入窗口
- Save: 打开文件窗口, 保存图像变换结果

## 功能实现

下面结合对应函数简要说明`MyImage`和`ImageWidget`类的功能实现

### `MyImage`类

#### Fourier Transform

首先, 在类中定义`datacomplex<double> *data`静态数组, 用于存储变换前后各像素的灰度和复频率信息. `data`初始化值为0, 当图像大小不是2的幂时, 不足部分不会写入data, 即用黑色补充图形

然后, 参照从`QImage`类中读取数据的相关方法, 在`readDataImage`函数中逐字节地将图像数据传到`data`数组中, 此处采用复数存储是为了便于后续FFT运算. 同时对RGB或ARGB格式图像, 则先将其转化为YIQ格式, 取Y为灰度值进行保存.

```cpp
unsigned char* pImageBytes = bits();//图像数据首地址
    int lineBytes = this->bytesPerLine();   //每行字节数
    unsigned char* pByte;

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
            //...
```



接下来, 使用递归算法实现FFT, 此处转动系数$Wn = e^{-2\pi/n}$, 与课本所给公式一致, 也与Matlab结果一致. 同时, 此处直接在输入数组`a`上进行变换, 可以节省空间

```cpp
void fft(complex<double> a[], int power)
```

再之后, 在`fourier`函数中, 依次对图像的各行各列进行FFT, 最后得到的结果经过`writeImage`函数写入新的`MyImage`类中.  `writeImage`函数有输入参数v, 是对数灰度变换函数$log_{v+1}(1+x*v)$中的参数, 通过调整v可以优化最终频谱图像显示. 

#### Inverse Fourier Transform

此处我实现了两个函数: `ifourier`基本步骤与上述一致, 只是中间使用的变换为`ifft`并且变换顺序由先行后列改为先列后行. 它同样需要读取图像数据然后再进行变换, 理论上说这时不可能实现的, 因为保存的傅里叶变换结果图丢失了相位信息, 无法还原. 

另外一个函数`ifourier_from_data`相比起`ifourer`去掉了读取数据步骤, 即仅使用`data`中存储的复数数据, 理论上可以顺利还原原始图像.

#### FFT->IFFT

简单地依次执行`fourier`和`ifourier_from_data`

```cpp
MyImage MyImage::fft_ifft()
{
    fourier();
    return  ifourier_from_data();
}
```

#### Histogram Equalization

使用与`readDataImage`类似的方法读取各像数灰度值, 并用`double histogram[256]`统计各灰度值像素的频率. 最后根据变换函数公式$T(r) = (L-1)\int_0^rp_r(w)dw$ 计算得到灰度图像转换函数(转换表)保存在`unsigned char T[256]`中. 再次遍历图像, 进行灰度转换即可.

### `ImageWidget`类

#### 图像绑定

在构造函数中, 主要工作是将输入的`MyImage`图像大小进行调整, 使其长或宽占满`GraphicView`, 然后将二者通过`QGraphicsScene`进行绑定

#### 实现滚轮控制放大

通过检测 `wheelEvent` 事件, 根据 `delta()` 值判断滚动方向, 调整 `m_scaleValue` 属性记录当前放大倍数, 并使用继承自 `QGraphicItem` 的 `setScale()` 函数进行放大

#### 鼠标拖动

依次检测`mousePressEvent`,  `mouseMoveEvent` 和 `mouseReleaseEvent`通过计算鼠标的`pos()`位置属性检测移动情况, 并对应地移动图像. 

## 实验结果

### 傅里叶变换

没有进行裁剪, 变换函数会用黑色(`complex<double>(0,0)`)填补剩余区域

![image-20220927213950406](.\assets\image-20220927213950406.png)

图像裁剪至边长为2的整数倍

![image-20220927214037987](.\assets\image-20220927214037987.png)

![image-20220927215847488](.\assets\image-20220927215847488.png)

#### 直接输入变换后的图形进行傅里叶反变换

![image-20220927220010880](.\assets\image-20220927220010880.png)

#### FFT -> IFFT

![image-20220927220202589](.\assets\image-20220927220202589.png)

![image-20220927220750631](.\assets\image-20220927220750631.png)

恢复的图像出现填补的黑色区域,(不影响恢复) 上图放大后对比如下

![image-20220927222102099](.\assets\image-20220927222102099.png)

#### Histogram Equalization

输入图像是原图经过亮度调整之后的

![image-20220927220839507](.\assets\image-20220927220839507.png)

![image-20220927221256431](.\assets\image-20220927221256431.png)

输入的图形是原图经过曝光调整之后的

![image-20220927221542164](.\assets\image-20220927221542164.png)

## 思考总结

1. 从边长不是二的幂次的图形的傅里叶变换结果来看, 填补黑边对傅里叶图形变换结果影响不大, 最后的结果只是拉伸了一下. 
2. 对频谱图直接使用反变结果验证了我们之前的猜想, 只用频谱幅度信息无法还原原图像. 而使用全部复频信息可以完全还原.
3. 直方图均衡化可以恢复图像的亮度, 但对于过曝图像, 由于数据丢失无法进行很好的恢复.
4. 在实现工程中, 关于旋转因子的次数, 经过实验发现, 正变换时$Wn = e^{-2\pi/n}$或$Wn = e^{2\pi/n}$都可以, 对 FFT->IFFT 没有影响, 只要反变换里符号相反就可以.
5. 中心化并不影响 FFT->IFFT 的结果, 这反映了频谱信号的周期性.

## 问题及后续改进

- 不同的变换可能需要输入各种参数, 此时虽然可以通过弹出窗口实现, 但这样无法实现动态调整. 后续可以增加属性调整窗框
- 无法进行旋转等操作. 
- 直方图均衡化暂时无法处理彩色图像, 同时只能处理8位色彩, 后续应该调整位根据图像性质自行适应
- 傅里叶变换对于边长不是2的幂次的图像必须要进行填补, 应该可以找到算法不改变大小或是自动删除黑边.