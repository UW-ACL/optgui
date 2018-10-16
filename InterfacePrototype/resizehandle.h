class Obstacle;

#ifndef RESIZEHANDLE_H
#define RESIZEHANDLE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include "obstacle.h"
#include <QtMath>

class ResizeHandle : public QGraphicsEllipseItem
{
public:
    ResizeHandle(Obstacle *parent);
    QRectF boundingRect() const;
    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
private:
    bool resize;
    Obstacle *circle;
    qreal radius;
};

#endif // RESIZEHANDLE_H
