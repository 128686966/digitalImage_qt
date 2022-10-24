#ifndef RAWIMAGE_H
#define RAWIMAGE_H

#include <qfile.h>
#include <qbytearray.h>
#include <qstring.h>
#include <memory>
#include "myimage.h"

using imgPtr = shared_ptr<MyImage>;

class RawImage
{
private:
    char* rawData{};
    int windowLevel{400};
    int windowWidth{200};
    qint32 height{0};
    qint32 width{0};
public:
    RawImage(QString rawFilename);
    RawImage(){}
    ~RawImage(){ delete[] rawData; }
    imgPtr raw2gry();
    void setWinLevel(int lev);
    void setWinWidth(int wid);
    int getWinLevel();
    int getWinWidth();
};

#endif // RAWIMAGE_H
