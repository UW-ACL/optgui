// TITLE:   Optimization_Interface/src/graphics/drone_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/drone_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>

namespace interface {

DroneGraphicsItem::DroneGraphicsItem(DroneModelItem *model,
                                     QGraphicsItem *parent,
                                     qreal size)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;
    this->initialize();
    this->size_ = size;
}

void DroneGraphicsItem::initialize() {
    // Set pen
    this->pen_ = QPen(Qt::black);
    this->pen_.setWidth(1);

    // Set brush
    this->brush_ = QBrush(Qt::yellow);

    // Set flags
    this->setFlags(QGraphicsItem::ItemSendsScenePositionChanges);
}

QRectF DroneGraphicsItem::boundingRect() const {
    return this->shape().boundingRect();
}

void DroneGraphicsItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Update pos
    this->setPos(*this->model_->pos_);

    // scale with view zoom level
    qreal scaling_factor = this->getScalingFactor();
    this->pen_.setWidthF(1.0 / scaling_factor);

    // Draw current course
    painter->setPen(this->pen_);
    painter->setBrush(this->brush_);

    painter->drawPath(this->shape());

    // Label with port
    if (this->model_->port_ != 0) {
        QPointF text_pos(this->mapFromScene(*this->model_->pos_));
        QFont font = painter->font();
        font.setPointSizeF(12.0 / scaling_factor);
        painter->setFont(font);
        qreal text_box_size = 50.0 / scaling_factor;
        painter->drawText(text_pos.x() - text_box_size,
                          text_pos.y() - text_box_size,
                          text_box_size * 2, text_box_size * 2,
                          Qt::AlignCenter,
                          QString::number(this->model_->port_));
    }
}

void DroneGraphicsItem::expandScene() {
    if (scene()) {
        // expand scene if item goes out of bounds
        QRectF newRect = this->sceneBoundingRect();
        QRectF rect = this->scene()->sceneRect();
        if (!rect.contains(newRect)) {
            this->scene()->setSceneRect(
                        this->scene()->sceneRect().united(newRect));
            if (!this->scene()->views().isEmpty()) {
                this->scene()->views().first()->setSceneRect(
                            this->scene()->sceneRect());
            }
        }
        this->update(this->boundingRect());
    }
}

QPainterPath DroneGraphicsItem::shape() const {
    QPainterPath path;
    QPolygonF poly;
    qreal size = this->size_ / this->getScalingFactor();
    poly << QPointF(0, size);
    poly << QPointF(size, 0);
    poly << QPointF(0, -size);
    poly << QPointF(-size, 0);
    poly << QPointF(0, size);
    path.addPolygon(poly);
    return path;
}

QVariant DroneGraphicsItem::itemChange(GraphicsItemChange change,
                                        const QVariant &value) {
    if (change == ItemScenePositionHasChanged && scene()) {
        // check to expand the scene
        this->expandScene();
    }
    return QGraphicsItem::itemChange(change, value);
}

qreal DroneGraphicsItem::getScalingFactor() const {
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace interface
