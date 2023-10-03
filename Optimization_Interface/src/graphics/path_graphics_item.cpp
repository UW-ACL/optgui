// TITLE:   Optimization_Interface/src/graphics/path_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/path_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>

namespace optgui {

PathGraphicsItem::PathGraphicsItem(PathModelItem *model,
                                   QGraphicsItem *parent,
                                   quint32 size)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;
    this->width_ = size;

    // Set pen
    this->pen_ = QPen(RED);
    this->pen_.setWidth(this->width_);

    // Set waypoints pen
    this->waypoints_pen_ = QPen(RED);
    this->waypoints_pen_.setWidth(2 * this->width_);

    // Set flags
    this->setFlags(QGraphicsItem::ItemSendsScenePositionChanges);
}

void PathGraphicsItem::setColor(QColor color) {
    QMutexLocker(&this->mutex_);
    this->pen_.setColor(color);
}

QRectF PathGraphicsItem::boundingRect() const {
    // return rough area of traj line
    return this->shape().boundingRect();
}

void PathGraphicsItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget) {
    // suppress unused options errors
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Draw current traj
    qreal scaling_factor = this->getScalingFactor();
    this->pen_.setWidthF(this->width_ / scaling_factor);
    quint32 size = this->model_->getSize();
    for (quint32 i = 1; i < size; i++) {
        painter->setPen(this->pen_);
        QLineF line(mapFromScene(this->model_->getPointAt(i - 1)),
                    mapFromScene(this->model_->getPointAt(i)));
        painter->drawLine(line);

        painter->setPen(this->waypoints_pen_);
        painter->drawEllipse(this->model_->getPointAt(i), 0.5 * this->width_ / scaling_factor,
                             0.5 * this->width_ / scaling_factor);
    }
}

QPainterPath PathGraphicsItem::shape() const {
    // return shape of traj line
    QPainterPath path;
    QPolygonF poly;
    quint32 size = this->model_->getSize();
    for (quint32 i = 0; i < size; i++) {
        poly << this->model_->getPointAt(i);
    }
    path.addPolygon(poly);
    return path;
}

QVariant PathGraphicsItem::itemChange(GraphicsItemChange change,
                                        const QVariant &value) {
    if (change == ItemScenePositionHasChanged && scene()) {
        // check redraw
        this->update(this->boundingRect());
    }
    return QGraphicsItem::itemChange(change, value);
}

qreal PathGraphicsItem::getScalingFactor() const {
    // get scaling zoom factor from view
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace optgui
