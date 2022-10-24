#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QGraphicsScene>


class ImageWidget : public QGraphicsItem
{
    //将图片显示到graphicsView控件上，并实现拖动等功能

public:
    //这里是否可以直接用QImage类？
    ImageWidget(QPixmap *pixmap, QGraphicsView *graphicsView);  //将输入图片绑定到输入的graphic view中

    void ResetItemPos();  //回到初始位置
    void wheelEvent(QGraphicsSceneWheelEvent *event);       //使用鼠标滚轮放大或缩小
    void mousePressEvent(QGraphicsSceneMouseEvent *event);  //右击回复原始大小
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);    //通过以上三个函数实现拖拽

    QRectF  boundingRect() const; //父类的两个纯虚函数
    void    paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPixmap m_pix;        //待绑定位图
    qreal m_scaleValue;   //记录当前放大倍数
    qreal m_scaleDefault; //初始放大倍数
    bool m_mousePressing; //记录鼠标是否按下
    QPointF m_startPos;   //鼠标按下位置

};

#endif // IMAGEWIDGET_H
