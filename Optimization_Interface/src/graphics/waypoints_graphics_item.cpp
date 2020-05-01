// TITLE:   Optimization_Interface/src/graphics/waypoints_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/waypoints_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineF>

#include "include/globals.h"

namespace optgui {

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
    this->resize_handles_ = new QVector<WaypointsResizeHandle *>();
}

WaypointsGraphicsItem::~WaypointsGraphicsItem() {
    // Delete resize handles
    for (WaypointsResizeHandle *handle : *this->resize_handles_) {
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
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);

    quint32 size = this->model_->getSize();
    for (quint32 i = 0; i < size; i++) {
        // Create new handles if necessary
        if (i+1 > this->resize_handles_->size()) {
            WaypointsResizeHandle *handle =
                    new WaypointsResizeHandle(
                        this->model_, i, this, this->size_);
            this->resize_handles_->append(handle);
            handle->show();
            this->expandScene();
        } else {
            // update handle pos
            this->resize_handles_->at(i)->updatePos();
        }
    }

    // Label with port
    /*
    if (!this->model_->points_->isEmpty() && this->model_->port_ != 0) {
        painter->setPen(Qt::black);
        QPointF text_pos(this->mapFromScene(*this->model_->points_->first()));
        QFont font = painter->font();
        qreal scaling_factor = this->getScalingFactor();
        font.setPointSizeF(12 / scaling_factor);
        painter->setFont(font);
        qreal text_box_size = 50.0 / scaling_factor;
        painter->drawText(text_pos.x() - text_box_size,
                          text_pos.y() - text_box_size,
                          text_box_size * 2, text_box_size * 2,
                          Qt::AlignCenter,
                          QString::number(this->model_->port_));
    }
    */
}

void WaypointsGraphicsItem::removeHandle(quint32 index) {
    this->resize_handles_->removeAt(index);
    this->model_->removePointAt(index);
    qint32 index_label = 0;
    for (WaypointsResizeHandle *handle : *this->resize_handles_) {
        handle->index_ = index_label;
        index_label++;
    }
}

QPainterPath WaypointsGraphicsItem::shape() const {
    QPainterPath path;
    QPolygonF poly;
    quint32 size = this->model_->getSize();
    for (quint32 i = 0; i < size; i++) {
        poly << this->model_->getPointAt(i);
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
        this->update(this->boundingRect());
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

}  // namespace optgui
