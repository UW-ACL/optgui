#include "resizehandle.h"

#include <obstacle.h>
#include <QDebug>

ResizeHandle::ResizeHandle(Obstacle *parent)
    : QGraphicsItem(parent)
{
    this->circle = parent;
    this->resize = false;
}

QRectF ResizeHandle::boundingRect() const
{
    return QRectF(-5, -5, 10, 10);
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
        //qDebug() << mouseEvent->pos().manhattanLength() << circle->boundingRect().center().manhattanLength();
        this->resize = true;
    }
    //QGraphicsItem::mousePressEvent(mouseEvent);
}

void ResizeHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (resize) {
        //qDebug() << "resizing" << (qFabs(mouseEvent->pos().x() - circle->pos().x()));
        this->circle->radius = (qFabs(mouseEvent->scenePos().x() - circle->scenePos().x()));
        //this->update();
        //this->circle->update();
        this->circle->scene()->update();
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
