// TITLE:   Optimization_Interface/src/graphics/ellipse_resize_handle.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/ellipse_resize_handle.h"

#include <QtMath>
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>

namespace optgui {

EllipseResizeHandle::EllipseResizeHandle(EllipseModelItem *model,
                                         QGraphicsItem *parent,
                                         qreal size)
    : QGraphicsEllipseItem(parent) {
    this->model_ = model;
    this->resize_ = false;
    this->setPen(QPen(Qt::black));
    this->setBrush(QBrush(Qt::white));
    this->size_ = size;
    this->setRect(-this->size_, -this->size_,
                  this->size_ * 2, this->size_ * 2);
}

void EllipseResizeHandle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->resize_ = true;
    }
}

void EllipseResizeHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->resize_ = false;
    }
}

void EllipseResizeHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (this->resize_) {
        this->model_->setRadius(qFabs(event->scenePos().x() -
                                parentItem()->scenePos().x()));
        this->expandScene();
    }
}

void EllipseResizeHandle::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget) {
    // scale with view
    qreal scaling_factor = this->getScalingFactor();
    qreal size = this->size_ / scaling_factor;
    QPen pen = this->pen();
    pen.setWidthF(1.0 / scaling_factor);
    this->setPen(pen);
    this->setRect(-size, -size, size * 2, size * 2);

    // paint
    QGraphicsEllipseItem::paint(painter, option, widget);
}

void EllipseResizeHandle::expandScene() {
    if (scene()) {
        // expand scene if item goes out of bounds
        QRectF newRect = parentItem()->sceneBoundingRect();
        QRectF rect = this->scene()->sceneRect();
        if (!rect.contains(newRect)) {
            this->scene()->setSceneRect(scene()->sceneRect().united(newRect));

            if (!this->scene()->views().isEmpty()) {
                this->scene()->views().first()->setSceneRect(
                            this->scene()->sceneRect());
            }
        }
        this->update(this->boundingRect());
    }
}

qreal EllipseResizeHandle::getScalingFactor() {
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace optgui
