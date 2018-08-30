#include "canvas.h"

#include <qDebug>

Canvas::Canvas(QObject *parent)
    : QGraphicsScene(parent)
{
    connect(this, SIGNAL(selectionChanged()), this, SLOT(bringToFront()));
}

void Canvas::bringToFront()
{
    if (!this->selectedItems().isEmpty()) {
        QGraphicsItem *selectedItem = this->selectedItems().first();
        QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

        qreal zValue = 0;
        foreach (QGraphicsItem *item, overlapItems) {
            if (item->zValue() >= zValue) {
                zValue = item->zValue() + 0.1;
            }
        }
        selectedItem->setZValue(zValue);
    }
}

void Canvas::drawBackground(QPainter *painter, const QRectF &rect) {
    qreal grids = 1;
    if (!this->views().isEmpty()) {
        grids = this->views().first()->matrix().m11();
    }
    //qDebug() << grids;

    QColor gridColor(Qt::gray);
    //gridColor.setAlpha(50);
    QPen gridLine(gridColor);

    QRectF bounding = this->sceneRect();
    qreal segment = 50;
    qreal gridWidth = 1;
    for (int i = 0; i < grids; i++) {
        drawGrids(painter, gridLine, gridWidth, bounding, segment);
        segment /= 2;
        gridWidth /= 2;
    }

    painter->drawRect(bounding);
    //painter->drawRect(itemsBoundingRect());
    //qDebug() << itemsBoundingRect().x() << itemsBoundingRect().y();
}

void Canvas::drawGrids(QPainter *painter, QPen pen, qreal width, QRectF bounding, qreal segment) {
    pen.setWidth(width);
    painter->setPen(pen);

    for (int i = bounding.left(); i < bounding.right(); i += segment) {
        painter->drawLine(i, bounding.top(), i, bounding.bottom());
    }
    for (int i = bounding.top(); i < bounding.bottom(); i += segment) {
        painter->drawLine(bounding.left(), i, bounding.right(), i);
    }
}
