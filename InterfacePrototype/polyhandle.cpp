#include "polyhandle.h"

PolyHandle::PolyHandle(QPointF *point, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
{
    this->radius = 7;
    this->point = point;
    this->setPos(*this->point);
    this->resize = false;
}

QRectF PolyHandle::boundingRect() const
{
    return QRectF(-this->radius, -this->radius, this->radius*2, this->radius*2);
}

void PolyHandle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    this->setRect(this->boundingRect());
    QRectF rec = this->boundingRect();
    QBrush brush(Qt::white);
    QPen pen(Qt::black);

    painter->setBrush(brush);
    painter->setPen(pen);

    painter->drawEllipse(rec);
}

void PolyHandle::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        this->resize = true;
    }
    //QGraphicsItem::mousePressEvent(mouseEvent);
}

void PolyHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (resize) {
        // TODO: act like pos is 0, 0
        qreal xPos = mouseEvent->scenePos().x() - parentItem()->scenePos().x();
        qreal yPos = mouseEvent->scenePos().y() - parentItem()->scenePos().y();
        this->point->setX(xPos);
        this->point->setY(yPos);
        this->setPos(QPointF(xPos, yPos));
        //this->update();
        //this->circle->update();
        this->scene()->update();
    }
    //QGraphicsItem::mouseMoveEvent(mouseEvent);
}

void PolyHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        this->resize = false;
    }
    //QGraphicsItem::mouseReleaseEvent(mouseEvent);
}
