#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>

class Obstacle : public QGraphicsItem
{
public:
    Obstacle(QGraphicsScene *parent = nullptr, qreal rad=50);

    QRectF boundingRect() const;
    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget);
    qreal radius;
private:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

};

#endif // OBSTACLE_H