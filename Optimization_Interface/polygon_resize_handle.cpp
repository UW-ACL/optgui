// TITLE:   Optimization_Interface/polygon_resize_handle.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include <QtMath>
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "polygon_resize_handle.h"

namespace interface {

PolygonResizeHandle::PolygonResizeHandle(QPointF *point, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
{
    this->point_ = point;
    this->resize_ = false;
    this->setPen(QPen(Qt::black));
    this->setBrush(QBrush(Qt::white));
    this->setRect(-POLYGON_HANDLE_SIZE / 2, -POLYGON_HANDLE_SIZE / 2, POLYGON_HANDLE_SIZE, POLYGON_HANDLE_SIZE);
}

void PolygonResizeHandle::updatePos()
{
    // Translate model point to local coordinates
    this->setPos(this->parentItem()->mapFromScene(*this->point_));
}

void PolygonResizeHandle::updateModel(QPointF diff)
{
    // Adjust model to new parent position
    *this->point_ += diff;
}

void PolygonResizeHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->resize_ = true;
    }
}

void PolygonResizeHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->resize_ = false;
    }
}

void PolygonResizeHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (this->resize_) {
        *this->point_ = event->scenePos();
        this->expandScene();
    }
}

void PolygonResizeHandle::expandScene() {
    if (scene()) {
        // expand scene if item goes out of bounds
        QRectF newRect = parentItem()->sceneBoundingRect();
        QRectF rect = this->scene()->sceneRect();
        if (!rect.contains(newRect)) {
            this->scene()->setSceneRect(scene()->sceneRect().united(newRect));
            if (!this->scene()->views().isEmpty())
            {
                this->scene()->views().first()->setSceneRect(this->scene()->sceneRect());
            }
        }
        this->scene()->update();
    }
}

}  // namespace
