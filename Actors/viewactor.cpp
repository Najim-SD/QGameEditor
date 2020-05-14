#include "viewactor.h"

ViewActor::ViewActor(QString _name, QRect r)
{
    name = _name;
    Actor::type = VIEW;
    QPen cp; cp.setColor(QColor(255,255,255)); cp.setWidth(0);
    setPen(cp);
    setRect(r);

    origin = QPoint(getWidth()/2, getHeight()/2);
    width = getWidth();
    height = getHeight();
    Actor::setTransformOriginPoint(origin);
}

int ViewActor::getWidth()
{
    return int(QGraphicsRectItem::boundingRect().width());
}

int ViewActor::getHeight()
{
    return int(QGraphicsRectItem::boundingRect().height());
}

QRectF ViewActor::boundingRect() const
{
    return QGraphicsRectItem::boundingRect();
}

void ViewActor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 1] draw the view rect
    QPen p = QGraphicsRectItem::pen();
    QRectF rf = QRectF(QPointF(QGraphicsRectItem::x()+0.5, QGraphicsRectItem::y()+0.5), QSizeF(QGraphicsRectItem::boundingRect().width()-1, QGraphicsRectItem::boundingRect().height()-1));
    painter->drawRect(rf);

    // 2] draw the purple selection box
    if(Actor::isSelected()){
        p.setColor(QColor(160,70,255)); p.setWidth(1);
        painter->setPen(p);
        painter->drawRect(rf);
    }
}

QPainterPath ViewActor::shape() const
{
    // the default shape has a filled rect, which we don't want, It should be a hollow rectangle
        //return QGraphicsRectItem::shape();

    QPainterPath rect;
    rect.addRect(QRectF(QPointF(QGraphicsRectItem::x()+0.5, QGraphicsRectItem::y()+0.5), QSizeF(QGraphicsRectItem::boundingRect().width()-1, QGraphicsRectItem::boundingRect().height()-1)));
    QPainterPathStroker ps; ps.setWidth(1);
    QPainterPath rectOutline = ps.createStroke(rect);
    return rectOutline;
}
