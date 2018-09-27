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

    //    painter->drawRect(itemsBoundingRect());
    //    painter->setPen(QPen(Qt::red));
    //    painter->drawRect(bounding);
    painter->drawText(1, -2, "0");
    //painter->drawText(0, 100, "+100");
    //painter->drawText(0, -100, "-100");

    //qDebug() << itemsBoundingRect().x() << itemsBoundingRect().y();
}

void Canvas::drawGrids(QPainter *painter, QPen pen, qreal width, QRectF bounding, qreal segment) {
    pen.setWidth(width);
    painter->setPen(pen);

    qreal top = (int)((bounding.top() - segment) / segment) * segment;
    qreal bot = (int)((bounding.bottom() + segment) / segment) * segment;
    qreal left = (int)((bounding.left() - segment) / segment) * segment;
    qreal right = (int)((bounding.right() + segment) / segment) * segment;

    // Right
    for (int i = 0; i <= bounding.right() + segment; i += segment) {
        painter->drawLine(i, top, i, bot);
    }

    // Left
    for (int i = 0; i >= bounding.left() - segment; i -= segment) {
        painter->drawLine(i, top, i, bot);
    }

    // Bottom
    for (int i = 0; i <= bounding.bottom() + segment; i += segment) {
        painter->drawLine(left, i, right, i);
    }

    // Top
    for (int i = 0; i >= bounding.top() - segment; i -= segment) {
        painter->drawLine(left, i, right, i);
    }
}
