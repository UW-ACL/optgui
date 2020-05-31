// TITLE:   Optimization_Interface/src/graphics/plane_resize_handle.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/plane_resize_handle.h"

#include <QtMath>
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "include/globals.h"

namespace optgui {

PlaneResizeHandle::PlaneResizeHandle(PlaneModelItem *model,
                                         bool isP2,
                                         QGraphicsItem *parent,
                                         qreal size)
    : QGraphicsEllipseItem(parent) {
    this->model_ = model;
    this->resize_ = false;
    this->setPen(QPen(Qt::black));
    this->setBrush(QBrush(Qt::white));
    this->size_ = size;
    this->isP2_ = isP2;
    this->setRect(-this->size_, -this->size_,
                  this->size_ * 2, this->size_ * 2);
}

void PlaneResizeHandle::setColor(const QColor color) {
    this->setBrush(QBrush(color));
}

void PlaneResizeHandle::updatePos() {
    // Translate model point to local coordinates
    if (this->isP2_) {
        this->setPos(this->parentItem()->mapFromScene(
                     this->model_->getP2()));
    } else {
        this->setPos(this->parentItem()->mapFromScene(
                     this->model_->getP1()));
    }
}

void PlaneResizeHandle::updateModel(QPointF diff) {
    // Adjust model to new parent position
    if (this->isP2_) {
        QPointF currPos = this->model_->getP2();
        this->model_->setP2(currPos + diff);
    } else {
        QPointF currPos = this->model_->getP1();
        this->model_->setP1(currPos + diff);
    }
}

void PlaneResizeHandle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->resize_ = true;
    }
}

void PlaneResizeHandle::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

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

void PlaneResizeHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->resize_ = false;
    }
}

void PlaneResizeHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (this->resize_) {
        QPointF eventPos = event->scenePos();
        if (this->isP2_) {
            this->model_->setP2(eventPos);
        } else {
            this->model_->setP1(eventPos);
        }
        this->update(this->boundingRect());
    }
}

int PlaneResizeHandle::type() const {
    return PLANE_HANDLE_GRAPHIC;
}

QPointF PlaneResizeHandle::getPoint() {
    if (this->isP2_) {
        return this->model_->getP2();
    } else {
        return this->model_->getP1();
    }
}

qreal PlaneResizeHandle::getScalingFactor() const {
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace optgui
