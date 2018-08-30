class ResizeHandle;

#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsScene>
#include <resizehandle.h>

class Obstacle : public QGraphicsItem
{
public:
    Obstacle(QGraphicsItem *parent = 0, qreal rad = 25);

    QRectF boundingRect() const;
    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget);
    qreal radius;
private:
    ResizeHandle *resizeHandle;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
};

#endif // OBSTACLE_H
