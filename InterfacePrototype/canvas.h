#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsScene>
#include <QPainter>
#include <QRectF>

class Canvas : public QGraphicsScene
{
    Q_OBJECT

public:
    Canvas(QObject *parent = nullptr);
    void drawBackground(QPainter *painter, const QRectF &rect);
};

#endif // CANVAS_H
