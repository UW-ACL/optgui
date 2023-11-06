// TITLE:   Optimization_Interface/src/graphics/cylinder_resize_handle.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/cylinder_resize_handle.h"

#include <QtMath>
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "include/globals.h"

namespace optgui {

CylinderResizeHandle::CylinderResizeHandle(CylinderModelItem *model,
                                         QGraphicsItem *parent,
                                         quint8 type,
                                         qreal size)
    : QGraphicsRectItem(parent) {
    // set model
    this->model_ = model;

    // set resize
    this->resize_ = false;

    // set graphics options
    this->setPen(QPen(Qt::black));
    this->setBrush(QBrush(Qt::white));
    this->size_ = size;
    this->type_ = type;

    // set size of circle
    this->setRect(-this->size_, -this->size_,
                  this->size_ * 2, this->size_ * 2);
}

void CylinderResizeHandle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->resize_ = true;
    }
}

void CylinderResizeHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->resize_ = false;
    }
}

void CylinderResizeHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (this->resize_) {
        // get rotation of mouse
        QLineF vector(event->scenePos(),
                      parentItem()->scenePos());

        qreal rotation = 360 - vector.angle();
        if (this->type_ == 0) {
            // adjust width
            this->model_->setWidth(vector.length());
        } else if (this->type_ == 1) {
            // adjust height
            this->model_->setHeight(vector.length());

            rotation -= 90;
            if (rotation < 0) {
                rotation += 360;
            }
        } else {
            // adjust trigger width
            this->model_->setTriggerWidth(vector.length()-this->model_->getWidth());
        }
        // set model rotation and graphical rotation
        this->model_->setRot(rotation);
        this->parentItem()->setRotation(rotation);

        // re-render cylinder graphic
        this->update(this->boundingRect());
    }
}

void CylinderResizeHandle::paint(QPainter *painter,
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
    QGraphicsRectItem::paint(painter, option, widget);
}

int CylinderResizeHandle::type() const {
    // return unique graphic type
    return CYLINDER_HANDLE_GRAPHIC;
}

qreal CylinderResizeHandle::getScalingFactor() const {
    // get scaling zoom factor from view
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace optgui
