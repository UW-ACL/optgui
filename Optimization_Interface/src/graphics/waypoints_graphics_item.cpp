// TITLE:   Optimization_Interface/src/graphics/waypoints_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/waypoints_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineF>

#include "include/globals.h"

namespace interface {

WaypointsGraphicsItem::WaypointsGraphicsItem(PathModelItem *model,
                                             QGraphicsItem *parent,
                                             qreal size)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;
    this->size_ = size;
    this->line_width_ = 4;
    this->initialize();
}

void WaypointsGraphicsItem::initialize() {
    // Set flags
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);

    // Initialize resize handles
    this->resize_handles_ = new QVector<PolygonResizeHandle *>();
}

WaypointsGraphicsItem::~WaypointsGraphicsItem() {
    // Delete resize handles
    for (PolygonResizeHandle *handle : *this->resize_handles_) {
        delete handle;
    }
    delete this->resize_handles_;
}

QRectF WaypointsGraphicsItem::boundingRect() const {
    return this->shape().boundingRect();
}

void WaypointsGraphicsItem::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *option,
                             QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Create new handles if necessary
    if (this->model_->points_->size() > this->resize_handles_->size()) {
        for (qint32 i = this->resize_handles_->size();
             i < this->model_->points_->size(); i++) {
            PolygonResizeHandle *handle =
                    new PolygonResizeHandle(
                        this->model_->points_->at(i), this, this->size_);
            this->resize_handles_->append(handle);
            handle->show();
        }
        this->expandScene();
    }

    // Set handle number
    qint32 index = 0;
    for (PolygonResizeHandle *handle : *this->resize_handles_) {
        handle->updatePos();
        handle->setIndex(index + 1);
        index++;
    }

    // Label with port
    if (!this->model_->points_->isEmpty() && this->model_->port_ != 0) {
        painter->setPen(Qt::black);
        QPointF text_pos(this->mapFromScene(*this->model_->points_->first()));
        painter->drawText(QRectF(text_pos.x(), text_pos.y(), 50, 15),
                          QString::number(this->model_->port_));
    }
}

void WaypointsGraphicsItem::removeHandle(PolygonResizeHandle *handle) {
    this->resize_handles_->removeOne(handle);
}

int WaypointsGraphicsItem::type() const {
    return WAYPOINTS_GRAPHIC;
}

QPainterPath WaypointsGraphicsItem::shape() const {
    QPainterPath path;
    QPolygonF poly;
    for (QPointF *point : *this->model_->points_) {
        poly << *point;
    }
    path.addPolygon(poly);
    return path;
}

void WaypointsGraphicsItem::expandScene() {
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

/*
QVariant WaypointsGraphicsItem::itemChange(GraphicsItemChange change,
                                         const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();

        // update model
        QPointF diff = newPos - this->scenePos();
        for (PolygonResizeHandle *handle : *this->resize_handles_) {
            handle->updateModel(diff);
        }

        // check to expand the scene
        this->expandScene();
    }
    return QGraphicsItem::itemChange(change, value);
}
*/

qreal WaypointsGraphicsItem::getScalingFactor() {
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace interface
