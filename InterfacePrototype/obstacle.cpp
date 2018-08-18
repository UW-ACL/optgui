#include "obstacle.h"

Obstacle::Obstacle(QGraphicsItem *parent, qreal rad)
    : QGraphicsItem(parent)
{
    this->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsScenePositionChanges);
    this->radius = rad;
    this->setSelected(false);
}

QRectF Obstacle::boundingRect() const
{
    return QRectF(0, 0, this->radius * 2, this->radius * 2);
}

void Obstacle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rec = this->boundingRect();
    QBrush brush(Qt::gray);
    QPen pen(Qt::black);

    if(this->isSelected()) {
        pen.setWidth(4);
    } else {
        pen.setWidth(2);
    }

    painter->setBrush(brush);
    painter->setPen(pen);

    painter->drawEllipse(rec);
}
