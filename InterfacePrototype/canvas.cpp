#include "canvas.h"

#include <QDebug>
#include <QGraphicsView>
Canvas::Canvas(QObject *parent)
    : QGraphicsScene(parent)
{

}

void Canvas::drawBackground(QPainter *painter, const QRectF &rect) {
    QPen gridLine(QColor(0xD3, 0xD3, 0xD3, 100));
    gridLine.setWidth(2);

    QRectF bounding = this->sceneRect();
    //qDebug() << "scene rect" << bounding.width() << bounding.height();
    //qDebug() << "view rect" << views().first()->width() << views().first()->height();

    // qreal segment = (bounding.right() - bounding.left()) / 10;
    float segment = 50;
    for (int i = bounding.left(); i < bounding.right(); i += segment) {
        painter->drawLine(i, bounding.top(), i, bounding.bottom());
    }
    for (int i = bounding.top(); i < bounding.bottom(); i += segment) {
        painter->drawLine(bounding.left(), i, bounding.right(), i);
    }

    painter->drawRect(bounding);
}
