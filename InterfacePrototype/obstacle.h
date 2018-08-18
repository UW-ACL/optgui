#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QGraphicsItem>
#include <QPainter>

class Obstacle : public QGraphicsItem
{
    Q_GADGET

public:
    Obstacle(QGraphicsItem *parent = 0, qreal rad = 25);

    QRectF boundingRect() const;
    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget);
    qreal radius;
};

#endif // OBSTACLE_H
