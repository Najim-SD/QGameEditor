#include "normalactor.h"

NormalActor::NormalActor(QString _name)
{
    name = _name;
    Actor::type = NORMAL;
    QPixmap pix(":/Resources/images/GE Actor in editor.png");
    tintImage = QPixmap(pix.size());
    setPixmap(pix);
    createTintImage(pixmap().size());

    originPointItem->setPos(QPoint(getWidth()/2, getHeight()/2));
    width = getWidth();
    height = getHeight();
    Actor::setTransformOriginPoint(originPointItem->pos());

    localTimeLine.setCurveShape(QTimeLine::LinearCurve);
    localTimeLine.setLoopCount(0);
    connect(&localTimeLine, SIGNAL(frameChanged(int)), this, SLOT(setFrame(int)));
}

int NormalActor::getWidth()
{
    return int(pixmap().width());
}

int NormalActor::getHeight()
{
    return int(pixmap().height());
}

void NormalActor::createTintImage(QSize s)
{
    if(pixmap().hasAlpha()){
        QBitmap mask = pixmap().mask();
        tintImage = QPixmap(s);
        tintImage.fill(QColor(0,0,0,0));
        QPainter p(&tintImage);
        p.setClipRegion(QRegion(mask));
        QPixmap pix(pixmap().size()); pix.fill(tint);
        p.drawImage(pix.rect(), pix.toImage(), pix.rect());
    }
    else{
        tintImage = QPixmap(s);
        tintImage.fill(tint);
    }

    Actor::update();
}

void NormalActor::addAnimation(Animation *animation)
{
    animations.append(animation);
    changeAnimation(animation->name, animationState);
}

int NormalActor::changeAnimation(QString animationName, AnimationState state)
{
    for(int i = 0; i < animations.size(); i++){
        if(animations[i]->name == animationName){
            // set animations[i] as current animation
            if(state != NO_CHANGE){
                animationState = state;
            }
            animindex = i;
            nframes = animations[i]->frames.size();

            localTimeLine.stop();
            changeAnimationFrameRate(animations[i]->frameRate);
            changeAnimationDirection(animationState);
            Actor::update();
            return 1;
        }
    }

    return 0;   // no such animation exists!
}

int NormalActor::changeAnimationDirection(AnimationState state)
{
    animationState = state;
    if(animations.isEmpty()) return 0;
    localTimeLine.stop();
    switch(state){
    case FORWARD:
        localTimeLine.setDirection(QTimeLine::Forward);
        localTimeLine.start();
        if(nframes == 1) setFrame(0);   // because QTimeLine does NOT start when range is [x to x+1]
        break;
    case BACKWARD:
        localTimeLine.setDirection(QTimeLine::Backward);
        localTimeLine.start();
        if(nframes == 1) setFrame(0);
        break;
    case STOPPED:
        setFrame(0);
    }
    return 1;
}

int NormalActor::changeAnimationFrameRate(int fps)
{
    if(animations.isEmpty()) return 0;

    animations[animindex]->frameRate = fps;
    QTimeLine::State originalState = localTimeLine.state();

    if(originalState == QTimeLine::Running) localTimeLine.setPaused(true);
    localTimeLine.setDuration((double(nframes) / fps) * 1000);
    localTimeLine.setFrameRange(0, nframes);
    if(originalState == QTimeLine::Running) localTimeLine.setPaused(false);

    return 1;
}

void NormalActor::setCompositionMode(QString mode)
{
    if(mode == "Normal (Source Over)") compositionMode = QPainter::CompositionMode_SourceOver;
    else if(mode == "Plus") compositionMode = QPainter::CompositionMode_Plus;
    else if(mode == "Multiply") compositionMode = QPainter::CompositionMode_Multiply;
    else if(mode == "Screen") compositionMode = QPainter::CompositionMode_Screen;
    else if(mode == "Overlay") compositionMode = QPainter::CompositionMode_Overlay;
    else if(mode == "Darken") compositionMode = QPainter::CompositionMode_Darken;
    else if(mode == "Lighten") compositionMode = QPainter::CompositionMode_Lighten;
    else if(mode == "Color Dodge") compositionMode = QPainter::CompositionMode_ColorDodge;
    else if(mode == "Color Burn") compositionMode = QPainter::CompositionMode_ColorBurn;
    else if(mode == "Hard Light") compositionMode = QPainter::CompositionMode_HardLight;
    else if(mode == "Soft Light") compositionMode = QPainter::CompositionMode_SoftLight;
    else if(mode == "Difference") compositionMode = QPainter::CompositionMode_Difference;
    else if(mode == "Exclusion") compositionMode = QPainter::CompositionMode_Exclusion;
    else if(mode == "Destination Over") compositionMode = QPainter::CompositionMode_DestinationOver;
    else if(mode == "Clear") compositionMode = QPainter::CompositionMode_Clear;
    else if(mode == "Source") compositionMode = QPainter::CompositionMode_Source;
    else if(mode == "Destination") compositionMode = QPainter::CompositionMode_Destination;
    else if(mode == "Source-In") compositionMode = QPainter::CompositionMode_SourceIn;
    else if(mode == "Destination-In") compositionMode = QPainter::CompositionMode_DestinationIn;
    else if(mode == "Source Out") compositionMode = QPainter::CompositionMode_SourceOut;
    else if(mode == "Destination Out") compositionMode = QPainter::CompositionMode_DestinationOut;
    else if(mode == "Source Atop") compositionMode = QPainter::CompositionMode_SourceAtop;
    else if(mode == "Destination Atop") compositionMode = QPainter::CompositionMode_DestinationAtop;
    else if(mode == "XOR") compositionMode = QPainter::CompositionMode_Xor;

    Actor::update();
}

QRectF NormalActor::boundingRect() const
{
    return QGraphicsPixmapItem::boundingRect();
}

void NormalActor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 1] draw the pixmap
    painter->setRenderHint(QPainter::SmoothPixmapTransform, antialiasing);

    painter->setCompositionMode(compositionMode);
    painter->drawPixmap(boundingRect(), pixmap(), QRect(QPoint(0,0), pixmap().size()));

    // tint the image
    if(colorFXStrenght > 0){
        painter->setCompositionMode(QPainter::CompositionMode_Multiply);    // this is original GE way of tinting with rgb values
        painter->setOpacity(colorFXStrenght * transp);
        painter->drawPixmap(boundingRect(), tintImage, QRect(QPoint(0,0), pixmap().size()));

        painter->setCompositionMode(compositionMode);
        painter->setOpacity(1);
    }

    // 2] draw the purple selection box
    if(Actor::isSelected()){
        QPen p; p.setColor(QColor(160,70,255)); p.setWidth(0);
        painter->setPen(p);
        QRectF r(boundingRect());
        painter->drawRect(r);
    }
}

QPainterPath NormalActor::shape() const
{
    return QGraphicsPixmapItem::shape();
}

void NormalActor::setPixmap(const QPixmap &pix)
{
    QGraphicsPixmapItem::setPixmap(pix);
    if(colorFXStrenght > 0){
        createTintImage(pixmap().size());
    }
}

void NormalActor::setTintColor(QColor color)
{
    tint = color;
    if(colorFXStrenght > 0){
        createTintImage(pixmap().size());
    }
}

void NormalActor::setTintStrength(qreal strength)
{
    if(colorFXStrenght <= 0 && strength > 0) createTintImage(pixmap().size());
    colorFXStrenght = strength;
    Actor::update();
}

void NormalActor::setFrame(int frameIndex)
{
    if(animations.isEmpty()) return;
    if(frameIndex > animations[animindex]->frames.size()-1 || frameIndex < 0) return;
    qreal ox = originPointItem->x() / width;
    qreal oy = originPointItem->y() / height;
    int olx = Actor::x;
    int oly = Actor::y;

    animpos = frameIndex;
    setPixmap(*(animations[animindex]->frames[animpos]->pixmap));

    // update...
    width = abs(getWidth() * xscale);
    height = abs(getHeight() * yscale);

    originPointItem->setPos(int(width * ox), int(height * oy));
    Actor::update();
    Actor::setPos(olx, oly);
    Actor::setRotation(Actor::rotation);
}
