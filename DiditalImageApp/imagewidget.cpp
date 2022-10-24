#include "imagewidget.h"

ImageWidget::ImageWidget(QPixmap *pixmap, QGraphicsView *graphicsView)
{
    m_pix = *pixmap;
    setAcceptDrops(true);   //检测鼠标悬浮
    m_mousePressing = false;
    int nwidth = graphicsView->width();
    int nheight = graphicsView->height();//获取Graphics View的宽高

    qreal nImgWidth = m_pix.width();
    qreal nImgHeight = m_pix.height();

    //选择较小的放大倍数，使图像占满View
    m_scaleDefault = std::min(nwidth*1.0/nImgWidth, nheight*1.0/nImgHeight);    setScale(m_scaleDefault);

    m_scaleValue = m_scaleDefault;
    setScale(m_scaleValue);

    //将ImageWidge显示在view上需要用到QGraphicsScene
    QGraphicsScene *scene = new QGraphicsScene;
    scene->addItem(this);
    graphicsView->setScene(scene);
    nwidth-=10;
    nheight-=10;
    graphicsView->setSceneRect(QRectF(- (nwidth / 2), - (nheight / 2),nwidth, nheight));  //View大小本身不应变化
    graphicsView->setFocus();   //将界面焦点设置到当前View
}

void ImageWidget::ResetItemPos()
{
    m_scaleValue = m_scaleDefault;
    setScale(m_scaleDefault);//缩放到一开始的自适应大小
    setPos(0,0);
}

void ImageWidget::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if((event->delta() > 0)&&(m_scaleValue >= 50))//最大放大到原始图像的50倍
        {
            return;
        }
        else if((event->delta() < 0)&&(m_scaleValue <= m_scaleDefault*0.5))//图像缩小到自适应大小的一半后之后就不继续缩小
        {
            return;//重置图片大小和位置，使之自适应控件窗口大小
        }
        else
        {
            qreal qrealOriginScale = m_scaleValue;
            if(event->delta() > 0)//鼠标滚轮向前滚动
            {
                m_scaleValue*=1.1;//每次放大10%
            }
            else
            {
                m_scaleValue*=0.9;//每次缩小10%
            }
            setScale(m_scaleValue);
            if(event->delta() > 0)
            {
                moveBy(-event->pos().x()*qrealOriginScale*0.1, -event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
            }
            else
            {
                moveBy(event->pos().x()*qrealOriginScale*0.1, event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
            }
    }
}

void ImageWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()== Qt::LeftButton)
        {
            m_startPos = event->pos();  //获取左击坐标，
            m_mousePressing = true;     //标记鼠标左键被按下
        }
        else if(event->button() == Qt::RightButton)
        {
            ResetItemPos();//右击鼠标重置大小
    }
}

void ImageWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_mousePressing)
    {
        QPointF point = (event->pos()-m_startPos)*m_scaleValue;
        moveBy(point.x(), point.y());
    }
}

void ImageWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_mousePressing = false;
}

QRectF ImageWidget::boundingRect() const
{
    return QRectF(-m_pix.width() / 2, -m_pix.height() / 2,
                  m_pix.width(), m_pix.height());
}

void ImageWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *,QWidget *)
{
    painter->drawPixmap(-m_pix.width() / 2, -m_pix.height() / 2, m_pix);
}

