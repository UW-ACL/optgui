#include "obstacle.h"


obstacle::obstacle(QGraphicsItem *parent)
{
    this->setFlags(QGraphicsItem::ItemIsMovable);
    this->setFlag(QGraphicsItem::ItemIsSelectable);
    this->radius = 50;
    this->setSelected(false);
}

QRectF obstacle::boundingRect() const
{
    return QRectF(QPointF(0, 0), QSizeF(this->radius * 2, this->radius * 2));
}

void obstacle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
