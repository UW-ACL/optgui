// TITLE:   Optimization_Interface/src/graphics/drone_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/drone_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>

namespace optgui {

DroneGraphicsItem::DroneGraphicsItem(DroneModelItem *model,
                                     QGraphicsItem *parent,
                                     qreal size)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;

    // Set pen
    this->pen_ = QPen(Qt::black);
    this->pen_.setWidth(1);

    // Set brush
    this->brush_ = QBrush(YELLOW);

    // Set flags
    this->setFlags(QGraphicsItem::ItemSendsScenePositionChanges);

    // set size
    this->size_ = size;
}

QRectF DroneGraphicsItem::boundingRect() const {
    // return area of drawn shape
    return this->shape().boundingRect();
}

void DroneGraphicsItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget) {
    // suppress unused options errors
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Update pos
    QPointF pos = this->model_->getPos();
    this->setPos(pos);

    // scale with view zoom level
    qreal scaling_factor = this->getScalingFactor();
    this->pen_.setWidthF(1.0 / scaling_factor);

    // Draw current course
    painter->setPen(this->pen_);
    painter->setBrush(this->brush_);

    painter->drawPath(this->shape());

    // Label with port
    if (this->model_->port_ != 0) {
        QPointF text_pos(this->mapFromScene(pos));
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

QPainterPath DroneGraphicsItem::shape() const {
    // create diamond shape to draw
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
        this->update(this->boundingRect());
    }
    return QGraphicsItem::itemChange(change, value);
}

qreal DroneGraphicsItem::getScalingFactor() const {
    // get the zoom scaling factor from the view
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace optgui
