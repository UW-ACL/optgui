class Obstacle;

#ifndef RESIZEHANDLE_H
#define RESIZEHANDLE_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <obstacle.h>
#include <QtMath>

class ResizeHandle : public QGraphicsItem
{
public:
    ResizeHandle(Obstacle *parent = 0);
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
};

#endif // RESIZEHANDLE_H
