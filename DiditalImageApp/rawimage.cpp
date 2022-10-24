#include "rawimage.h"
#include <fstream>
#include <iostream>
#include <QDebug>

template <typename T> // T must be integer type
T EndTransform(T x)
{
    size_t n = sizeof(T) / sizeof(uint8_t); // 2,4,8

    T res;
    uint8_t* p = (uint8_t*) &res;
    T mask = static_cast<T>(0xFF);

    for (size_t i = 0; i < n; i++){
        int offset = 8 * i;
        p[i] = (x & (mask << offset)) >> offset;
    }

    return res;
}

RawImage::RawImage(QString rawFilename)
{
    ifstream datafile;
    datafile.open(rawFilename.toStdString().c_str(), std::ofstream::binary);
    datafile.read((char*)(&width), sizeof(int));
    width = EndTransform<int>(width);
    datafile.seekg(4, ios::beg);
    datafile.read((char*)(&height), sizeof(int));
    height = EndTransform<int>(height);
    datafile.seekg(8, ios::beg);
    rawData = new char[width*height*2];
    datafile.read(rawData, width * height * 2 *sizeof (char));
    datafile.close();
}


imgPtr RawImage::raw2gry()
{
    imgPtr destImage = make_shared<MyImage>(width, height, QImage::Format_Indexed8);
    for(int i = 0; i < height; i++){
        for(int j = 0; j< width; j++){
            char upper = rawData[(i * width + j ) * 2 + 1];
            upper = upper & 0x0f; // 保留低四位
            char lower = rawData[(i * width + j ) * 2];
            int rawGray = (static_cast<int>(upper) << 8) + static_cast<int>(lower);
            int g = static_cast<int>(floor(255.0/windowWidth * (rawGray-windowLevel+windowWidth/2.0)));
            if ( g > 255) g = 255;
            if (g < 0) g = 0;
            *destImage->get_p(i,j) = static_cast<unsigned char>(g);
        }
    }
    return destImage;
}

void RawImage::setWinLevel(int lev){ windowLevel = lev; }
void RawImage::setWinWidth(int wid){ windowWidth = wid; }
int RawImage::getWinLevel(){ return windowLevel; }
int RawImage::getWinWidth(){ return windowWidth; }
