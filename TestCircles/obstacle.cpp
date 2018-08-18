#include "obstacle.h"


Obstacle::Obstacle(QGraphicsScene *parent, qreal rad)
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

QVariant Obstacle::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        rect.setBottom(rect.bottom()  - radius*2);
        rect.setRight(rect.right() - radius*2);

        if (!rect.contains(newPos)) {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
            return newPos;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}
