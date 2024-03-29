#ifndef VIEWACTOR_H
#define VIEWACTOR_H

#include <QObject>
#include <QCursor>
#include <QImage>
#include <QPainter>
#include <QTimer>

#include "Actors/actor.h"

class ViewActor : public Actor, public QGraphicsRectItem
{
    Q_OBJECT
public:
    ViewActor(QString _name, QRect r);
    int getWidth();
    int getHeight();

    // Pure Virtual functions...
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setTintColor(QColor color);
    void setTintStrength(qreal strength);

    // overriden
    QPainterPath shape() const;
};

#endif // VIEWACTOR_H
