#ifndef POLYOBSTACLE_H
#define POLYOBSTACLE_H

#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "polyhandle.h"
#include <QPolygonF>
#include <QVector>
#include <QPointF>

class PolyObstacle : public QGraphicsPolygonItem
{
public:
    explicit PolyObstacle(QVector<QPointF*> *pointVector, QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget);
private:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    QVector<PolyHandle *> *handleVector;
    void setPoly();
    QVector<QPointF*> *pointVector;
    void showHandles();
    void hideHandles();
};

#endif // POLYOBSTACLE_H
