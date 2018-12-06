#ifndef LINEOBSTACLE_H
#define LINEOBSTACLE_H


#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "polyhandle.h"
#include <QVector>
#include <QPointF>

class LineObstacle : public QGraphicsLineItem
{
public:
    explicit LineObstacle(QVector<QPointF*> *pointVector, QGraphicsItem *parent = 0);
    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget);
private:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    QVector<PolyHandle *> *handleVector;
    void pointsToLine();
    QVector<QPointF*> *pointVector;
    void showHandles();
    void hideHandles();
};

#endif // LINEOBSTACLE_H
