#ifndef POLYHANDLE_H
#define POLYHANDLE_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QGraphicsScene>

class PolyHandle : public QGraphicsItem
{
public:
    explicit PolyHandle(QPointF *point, QGraphicsItem *parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QPointF *point;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
private:
    bool resize;
    qreal radius;
};

#endif // POLYHANDLE_H
