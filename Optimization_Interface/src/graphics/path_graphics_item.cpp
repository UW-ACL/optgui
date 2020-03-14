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
    this->initialize();
}

void PathGraphicsItem::initialize() {
    // Set pen
    this->pen_ = QPen(RED);
    this->pen_.setWidth(this->width_);

    // Set flags
    this->setFlags(QGraphicsItem::ItemSendsScenePositionChanges);
}

void PathGraphicsItem::setColor(QColor color) {
    this->pen_.setColor(color);
}

QRectF PathGraphicsItem::boundingRect() const {
    return this->shape().boundingRect();
}

void PathGraphicsItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Draw current course
    qreal scaling_factor = this->getScalingFactor();
    this->pen_.setWidthF(this->width_ / scaling_factor);
    painter->setPen(this->pen_);
    quint32 size = this->model_->getSize();
    for (quint32 i = 1; i < size; i++) {
        QLineF line(mapFromScene(this->model_->getPointAt(i - 1)),
                    mapFromScene(this->model_->getPointAt(i)));
        painter->drawLine(line);
    }
}

void PathGraphicsItem::expandScene() {
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

QPainterPath PathGraphicsItem::shape() const {
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
        // check to expand the scene
        this->expandScene();
    }
    return QGraphicsItem::itemChange(change, value);
}

qreal PathGraphicsItem::getScalingFactor() {
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace optgui
