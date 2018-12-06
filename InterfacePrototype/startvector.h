#ifndef STARTVECTOR_H
#define STARTVECTOR_H

#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "polyhandle.h"
#include <QVector>
#include <QPointF>

class StartVector : public QGraphicsLineItem
{
public:
    explicit StartVector(QVector<QPointF*> *pointVector, QGraphicsItem *parent = 0);
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

#endif // STARTVECTOR_H
