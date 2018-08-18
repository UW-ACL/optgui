#ifndef CANVAS_H
#define CANVAS_H

#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

#include <QGraphicsScene>
#include <QPainter>
#include <QRectF>

class Canvas : public QGraphicsScene
{
public:
    Canvas(QObject *parent = nullptr);
    void drawBackground(QPainter *painter, const QRectF &rect);

protected:
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // CANVAS_H
