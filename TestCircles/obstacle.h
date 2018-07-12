#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QPainter>
#include <QGraphicsItem>

class obstacle : public QGraphicsItem
{
public:
    obstacle(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const;
    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget);
    float radius;
};

#endif // OBSTACLE_H
