// TITLE:   Optimization_Interface/polygon_resize_handle.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "polygon_resize_handle.h"

#include <QtMath>
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "globals.h"

namespace interface {

PolygonResizeHandle::PolygonResizeHandle(QPointF *point, QGraphicsItem *parent, quint32 size)
    : QGraphicsEllipseItem(parent) {
    this->point_ = point;
    this->resize_ = false;
    this->setPen(QPen(Qt::black));
    this->setBrush(QBrush(Qt::white));
    this->size_ = size;
    this->setRect(-this->size_ / 2, -this->size_ / 2,
                  this->size_, this->size_);
}

void PolygonResizeHandle::setColor(const QColor color) {
    this->setBrush(QBrush(color));
}

void PolygonResizeHandle::updatePos() {
    // Translate model point to local coordinates
    this->setPos(this->parentItem()->mapFromScene(*this->point_));
}

void PolygonResizeHandle::updateModel(QPointF diff) {
    // Adjust model to new parent position
    *this->point_ += diff;
}

void PolygonResizeHandle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->resize_ = true;
    }
}

void PolygonResizeHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->resize_ = false;
    }
}

void PolygonResizeHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (this->resize_) {
        *this->point_ = event->scenePos();
        this->expandScene();
    }
}

int PolygonResizeHandle::type() const {
    return HANDLE_GRAPHIC;
}

QPointF *PolygonResizeHandle::getPoint() {
    return this->point_;
}

void PolygonResizeHandle::expandScene() {
    if (scene()) {
        // expand scene if item goes out of bounds
        QRectF newRect = this->parentItem()->sceneBoundingRect();
        QRectF rect = this->scene()->sceneRect();
        if (!rect.contains(newRect)) {
            this->scene()->setSceneRect(scene()->sceneRect().united(newRect));
            if (!this->scene()->views().isEmpty()) {
                this->scene()->views().first()->setSceneRect(
                            this->scene()->sceneRect());
            }
        }
        this->scene()->update();
    }
}

}  // namespace interface
