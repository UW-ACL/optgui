#include "polydot.h"

PolyDot::PolyDot(QPointF *point, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
{
    this->radius = 7;
    this->point = point;
    this->setPos(*this->point);
}

QRectF PolyDot::boundingRect() const
{
    return QRectF(-this->radius, -this->radius, this->radius*2, this->radius*2);
}

void PolyDot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    this->setRect(this->boundingRect());
    QRectF rec = this->boundingRect();
    QBrush brush(Qt::white);
    QPen pen(Qt::black);

    painter->setBrush(brush);
    painter->setPen(pen);

    painter->drawEllipse(rec);
}
