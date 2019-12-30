// TITLE:   Optimization_Interface/src/graphics/point_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/point_graphics_item.h"

#include <QGraphicsScene>
#include <QtMath>
#include <QGraphicsView>

#include "include/globals.h"

namespace interface {

PointGraphicsItem::PointGraphicsItem(PointModelItem *model,
                                     QGraphicsItem *parent,
                                     qreal size)
    : QGraphicsEllipseItem(parent) {
    // Set model
    this->model_ = model;
    this->radius_ = size;
    this->resize_ = false;
    this->setPen(QPen(Qt::black));
    this->setBrush(QBrush(Qt::red));
    this->setRect(-this->radius_, -this->radius_,
                  this->radius_ * 2, this->radius_ * 2);
    this->setPos(*this->model_->pos_);
}

void PointGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->resize_ = true;
    }
}

void PointGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->resize_ = false;
    }
}

void PointGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (this->resize_) {
        QPointF eventPos = event->scenePos();
        this->model_->pos_->setX(eventPos.x());
        this->model_->pos_->setY(eventPos.y());
        this->setPos(*this->model_->pos_);
        this->expandScene();
    }
}

void PointGraphicsItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // scale with view
    qreal scaling_factor = this->getScalingFactor();
    qreal size = this->radius_ / scaling_factor;
    QPen pen = this->pen();
    pen.setWidthF(1.0 / scaling_factor);
    this->setPen(pen);
    this->setRect(-size, -size, size * 2, size * 2);

    // paint
    QGraphicsEllipseItem::paint(painter, option, widget);

    // Label with port
    if (this->model_->port_ != 0) {
        QPointF text_pos(this->mapFromScene(*this->model_->pos_));
        QFont font = painter->font();
        font.setPointSizeF(12 / scaling_factor);
        painter->setFont(font);
        qreal text_box_size = 50.0 / scaling_factor;
        painter->drawText(text_pos.x() - text_box_size,
                          text_pos.y() - text_box_size,
                          text_box_size * 2, text_box_size * 2,
                          Qt::AlignCenter,
                          QString::number(this->model_->port_));
    }
}

int PointGraphicsItem::type() const {
    return POINT_GRAPHIC;
}

void PointGraphicsItem::expandScene() {
    if (scene()) {
        // expand scene if item goes out of bounds
        QRectF newRect = this->sceneBoundingRect();
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

qreal PointGraphicsItem::getScalingFactor() const {
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace interface
