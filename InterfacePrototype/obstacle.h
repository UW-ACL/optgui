class ResizeHandle;

#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QGraphicsScene>
#include "resizehandle.h"
#include <QGraphicsView>

class Obstacle : public QGraphicsEllipseItem
{
public:
    Obstacle(qreal rad = 25, QGraphicsItem *parent = 0);

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
