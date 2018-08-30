#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsScene>
#include <QPainter>
#include <QRectF>
#include <QGraphicsItem>
#include <QGraphicsView>

class Canvas : public QGraphicsScene
{
    Q_OBJECT

    void drawGrids(QPainter *painter, QPen pen, qreal width, QRectF bounding, qreal segment);
public:
    Canvas(QObject *parent = nullptr);
    void drawBackground(QPainter *painter, const QRectF &rect);
private slots:
    void bringToFront();
};

#endif // CANVAS_H
