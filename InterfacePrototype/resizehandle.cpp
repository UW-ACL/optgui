#include "resizehandle.h"

ResizeHandle::ResizeHandle(Obstacle *parent)
    : QGraphicsItem(parent)
{
    this->circle = parent;
    this->resize = false;
    this->radius = 7;
}

QRectF ResizeHandle::boundingRect() const
{
    return QRectF(-this->radius, -this->radius, this->radius*2, this->radius*2);
}

void ResizeHandle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rec = this->boundingRect();
    QBrush brush(Qt::white);
    QPen pen(Qt::black);

    painter->setBrush(brush);
    painter->setPen(pen);

    painter->drawEllipse(rec);
}

void ResizeHandle::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        this->resize = true;
    }
    //QGraphicsItem::mousePressEvent(mouseEvent);
}

void ResizeHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (resize) {
        this->circle->radius = (qFabs(mouseEvent->scenePos().x() - circle->scenePos().x()));
        //this->update();
        //this->circle->update();
        this->scene()->update();
        //this->circle->scene()->update();
    }
    //QGraphicsItem::mouseMoveEvent(mouseEvent);
}

void ResizeHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        this->resize = false;
    }
    //QGraphicsItem::mouseReleaseEvent(mouseEvent);
}
