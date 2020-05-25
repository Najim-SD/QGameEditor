#ifndef ACTOR_H
#define ACTOR_H

#include "helper.h"

#include <QObject>
#include <QDebug>
#include <QCursor>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QGraphicsColorizeEffect>
#include <QTimeLine>

enum ActorType{NORMAL, VIEW, CANVAS, WIRE_FRAME_REGION, FILLED_REGION, LABEL /*,TILE*/};

class Actor : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    Actor();

    // ...
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    QPoint pos();
    QPoint scenePos();
    void setPos(int nx, int ny);
    void setPos(QPointF f);
    void setX(int nx);
    void setY(int ny);
    void setXScale(qreal newXScale);
    void setYScale(qreal newYScale);
    void setRotation(qreal ro);

    // Actor Properties
    ActorType type;
    QString name;
    QString clonename;
    int cloneindex;

    bool createAtStartup;

    int x, y;
    int xprevious, yprevious;
    QGraphicsEllipseItem * originPointItem;
    int xscreen, yscreen;
    int xvelocity{0}, yvelocity{0};
    int angle{0}, directional_velocity{0};
    int width, height;
    qreal zdepth{0}, transp{1};
    qreal rotation{0};
    qreal xscale{1}, yscale{1};
    QColor tint;
    qreal colorFXStrenght{0};
    // user vars...

    // QGraphicsItem Interface
    virtual QRectF boundingRect() const = 0;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) = 0;

    // Editor stuff
        // for mouse movement with SHIFT key modifier
    bool xSnap{false};
    bool ySnap{false};
    bool isLocked{false};

    // Effects
    QGraphicsColorizeEffect * colorEffect;
    void setTintColor(QColor color);
    void setTintStrength(qreal strength);

signals:
    // in-Editor signals
    void actorClicked(Actor *);
    void positionChanged(Actor *);
    void actorSelectionChanged(Actor * actor, bool state);
    void deleteActor(Actor*);
    void snappingStateChanged(Actor*);

public slots:
    void lockUnLock();  // locks if unlocked and vice-versa
    void hideUnHide();
    void sendDeleteSignal();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    // Mouse
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    // Keyboard
    void keyPressEvent(QKeyEvent *event);
};

#endif // ACTOR_H
