// TITLE:   Optimization_Interface/src/graphics/drone_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "../../include/graphics/drone_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>

namespace interface {

DroneGraphicsItem::DroneGraphicsItem(DroneModelItem *model,
                                     QGraphicsItem *parent,
                                     qint32 size)
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
    this->setPos(*this->model_->point_);

    // Draw current course
    painter->setPen(this->pen_);
    painter->setBrush(this->brush_);

    painter->drawPath(this->shape());

    // Label with port
    if (this->model_->port_ != 0) {
        QPointF text_pos(this->mapFromScene(*this->model_->point_));
        painter->drawText(QRectF(text_pos.x(), text_pos.y(), 50, 15),
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
        this->scene()->update();
    }
}

QPainterPath DroneGraphicsItem::shape() const {
    QPainterPath path;
    QPolygonF poly;
    qint32 s = this->size_;
    poly << QPoint(0, s);
    poly << QPoint(s, 0);
    poly << QPoint(0, -s);
    poly << QPoint(-s, 0);
    poly << QPoint(0, s);
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

}  // namespace interface
