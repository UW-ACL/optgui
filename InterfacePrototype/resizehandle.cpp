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
    return QRectF(0, 0, 10, 10);
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
}

void ResizeHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (resize) {
        //qDebug() << "resizing" << mouseEvent->pos().manhattanLength() << circle->boundingRect().center().manhattanLength();
        //this->circle->radius = (qFabs(mouseEvent->pos().manhattanLength() - circle->boundingRect().center().manhattanLength()));
        //update();
    }
}

void ResizeHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        this->resize = false;
    }
}
