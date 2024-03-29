#include "actor.h"

Actor::Actor()
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    setAcceptHoverEvents(true);


    //default RGB colorEffect in GE
    tint = QColor(255,255,255);

    // origin point item
    originPointItem = new PointHandleItem(QRect(0,0,6,6), this);
    connect(originPointItem, SIGNAL(pointChanged()), this, SLOT(updateOriginPoint()));
    connect(originPointItem, SIGNAL(posChanging()), this, SLOT(emitOriginChange()));
}

QPoint Actor::pos()
{
    QPoint po = QGraphicsItem::pos().toPoint();
    po.setX(po.x() + originPointItem->finalPosition.x());
    po.setY(po.y() + originPointItem->finalPosition.y());
    return po;
}

QPoint Actor::scenePos()
{
    QPoint po = QGraphicsItem::scenePos().toPoint();
    po.setX(po.x() + originPointItem->finalPosition.x());
    po.setY(po.y() + originPointItem->finalPosition.y());
    return po;
}

void Actor::setPos(int nx, int ny)
{
    Actor::x = nx;
    Actor::y = ny;
    QGraphicsItem::setPos(nx - originPointItem->finalPosition.x(), ny - originPointItem->finalPosition.y());
}

void Actor::setPos(QPointF f)
{
    Actor::setPos(f.toPoint().x(), f.toPoint().y());
}

void Actor::setX(int nx)
{
    Actor::x = nx;
    QGraphicsItem::setX(nx - originPointItem->finalPosition.x());
}

void Actor::setY(int ny)
{
    Actor::y = ny;
    QGraphicsItem::setY(ny - originPointItem->finalPosition.y());
}

void Actor::setXScale(qreal newXScale)
{
    QTransform tr;

    tr.translate(originPointItem->finalPosition.x(), originPointItem->finalPosition.y());  // to origin
    tr.rotate(rotation);
    tr.scale(newXScale, yscale);
    tr.translate(-originPointItem->finalPosition.x(), -originPointItem->finalPosition.y()); // and back

    setTransform(tr);
    xscale = newXScale;
    width = abs(getWidth() * xscale);
}

void Actor::setYScale(qreal newYScale)
{
    QTransform tr;

    tr.translate(originPointItem->finalPosition.x(), originPointItem->finalPosition.y());  // to origin
    tr.rotate(rotation);
    tr.scale(xscale, newYScale);
    tr.translate(-originPointItem->finalPosition.x(), -originPointItem->finalPosition.y()); // and back

    setTransform(tr);
    yscale = newYScale;
    height = abs(getHeight() * yscale);
}

void Actor::setRotation(qreal ro)
{
    QTransform tr;

    tr.translate(originPointItem->finalPosition.x(), originPointItem->finalPosition.y());
    tr.rotate(ro);
    tr.scale(xscale, yscale);
    tr.translate(-originPointItem->finalPosition.x(), - originPointItem->finalPosition.y());

    setTransform(tr);
    rotation = ro;
}

void Actor::lockUnLock()
{
    setFlag(QGraphicsItem::ItemIsMovable, isLocked);
    setFlag(QGraphicsItem::ItemIsFocusable, isLocked);
    setFlag(QGraphicsItem::ItemIsSelectable, isLocked);

    isLocked = !isLocked;
}

void Actor::hideUnHide()
{
    setVisible(!isVisible());
}

void Actor::sendDeleteSignal()
{
    emit deleteActor(this); // :(
}

void Actor::updateOriginPoint()
{
    setTransformOriginPoint(originPointItem->finalPosition);

    QTransform tr;

    tr.translate(originPointItem->finalPosition.x(), originPointItem->finalPosition.y());  // to origin
    tr.rotate(rotation);
    tr.scale(xscale, yscale);
    tr.translate(-originPointItem->finalPosition.x(), - originPointItem->finalPosition.y()); // and back

    setTransform(tr);

    // then set position to old point
    setPos(originPointItem->releasePoint);
    emit positionChanged(this);     // because it doesn't update the GUI when rotation = 0
}

void Actor::emitOriginChange()
{
    emit originChanged(this);
}

// Protected --------------------------------------------------------------------
//     -----------------------------------------------------------------------

QVariant Actor::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange){
        QPoint newPos = value.toPointF().toPoint();
        newPos.rx() += originPointItem->finalPosition.x();
        newPos.ry() += originPointItem->finalPosition.y();

        Actor::xprevious = Actor::x;
        Actor::yprevious = Actor::y;

        // handle mouse move snapping
        if(ySnap){  // Snap to Y Axis
            newPos.rx() = xprevious;
        }else if(xSnap) {  // Snap to X Axis
            newPos.ry() = yprevious;
        }

        Actor::x = newPos.rx();
        Actor::y = newPos.ry();

        newPos.rx() -= originPointItem->finalPosition.x();
        newPos.ry() -= originPointItem->finalPosition.y();

        return QPointF(newPos);
    }
    else if(change == ItemPositionHasChanged){
        // emit a pos change signal

        emit positionChanged(this);
    }

    if(change == ItemSelectedHasChanged){
        bool selected = value.toBool();
        originPointItem->setVisible(selected);
        // emit the change
        emit actorSelectionChanged(this, selected);
    }
    return QGraphicsItem::itemChange(change, value);
}

void Actor::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverEnterEvent(event);
    setCursor(QCursor(Qt::PointingHandCursor));
}

void Actor::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverLeaveEvent(event);
    setCursor(QCursor(Qt::ArrowCursor));
}

void Actor::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    switch(event->button()){
        default:
        case Qt::LeftButton:
            emit actorClicked(this);
        break;
    }
    setCursor(QCursor(Qt::PointingHandCursor));
}

void Actor::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    setCursor(QCursor(Qt::PointingHandCursor));
    // reset snap axis
    xSnap = ySnap = false;
    emit snappingStateChanged(this);
}

void Actor::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Actor is moved by mouse
    QGraphicsItem::mouseMoveEvent(event);
    if(event->modifiers() == Qt::SHIFT){
        if(! (xSnap || ySnap)){
            // Move actor on a straight line
            int xDiff = abs(xprevious - Actor::x);
            int yDiff = abs(yprevious - Actor::y);
            if(xDiff < yDiff) ySnap = true;
            else xSnap = true;
            emit snappingStateChanged(this);
        }
    }else if(xSnap || ySnap){
        // reset snap axis
        xSnap = ySnap = false;
        emit snappingStateChanged(this);
    }
}

void Actor::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *lockAction = menu.addAction(isLocked ? "UnLock Actor" : "Lock Actor");
    QAction *hideAction = menu.addAction(isVisible() ? "Hide Actor" : "unHide Actor");
    QAction *deleteAction = menu.addAction("Delete Actor");
    QAction *selectedAction = menu.exec(event->screenPos());

    // connect those actions to slots
    connect(lockAction, &QAction::triggered, this, &Actor::lockUnLock);
    connect(hideAction, &QAction::triggered, this, &Actor::hideUnHide);
    connect(deleteAction, &QAction::triggered, this, &Actor::sendDeleteSignal);

    if(selectedAction){
        selectedAction->trigger();
    }

}

void Actor::keyPressEvent(QKeyEvent *event)
{
    // if in editor mode then react to arrow keys to move the actor
    if(isSelected()){
        switch(event->key()){
        case Qt::Key_Right:
            setX(x+1);
        break;
        case Qt::Key_Left:
            setX(x-1);
        break;
        case Qt::Key_Up:
            setY(y-1);
        break;
        case Qt::Key_Down:
            setY(y+1);
        break;
        }
    }
}
