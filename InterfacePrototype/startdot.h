#ifndef STARTDOT_H
#define STARTDOT_H


#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>

class StartDot : public QGraphicsEllipseItem
{
public:
    explicit StartDot(QPointF *point, QColor color, QGraphicsItem *parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QPointF *point;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
private:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    bool resize;
    qreal radius;
    QColor color;
};

#endif // STARTDOT_H
