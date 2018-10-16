#ifndef POLYDOT_H
#define POLYDOT_H

#include <QPainter>
#include <QRectF>
#include <QPointF>
#include <QGraphicsItem>

class PolyDot : public QGraphicsItem
{
public:
    PolyDot(QPointF *point, QGraphicsItem *parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QPointF *point;
    qreal radius;
};

#endif // POLYDOT_H
