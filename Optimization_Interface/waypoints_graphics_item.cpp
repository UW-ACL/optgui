// TITLE:   Optimization_Interface/waypoints_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "waypoints_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineF>

#include "globals.h"

namespace interface {

WaypointsGraphicsItem::WaypointsGraphicsItem(QVector<QPointF *> *points,
                                   QGraphicsItem *parent)
    : QGraphicsItem(parent) {
    // Set model
    this->waypoints_ = points;
    this->initialize();
}

void WaypointsGraphicsItem::initialize() {
    // Set pen
    this->pen_ = QPen(Qt::blue);
    this->pen_.setWidth(1);

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
    if (this->waypoints_->size() > this->resize_handles_->size()) {
        for (qint32 i = this->resize_handles_->size();
             i < this->waypoints_->size(); i++) {
            PolygonResizeHandle *handle =
                    new PolygonResizeHandle(this->waypoints_->at(i), this);
            this->resize_handles_->append(handle);
            handle->show();
        }
        this->expandScene();
    }

    // Draw connecting path
    painter->setPen(this->pen_);
    for (qint32 i = 1; i < this->waypoints_->length(); i++) {
        QLineF line(mapFromScene(*this->waypoints_->at(i-1)),
                    mapFromScene(*this->waypoints_->at(i)));
        painter->drawLine(line);
    }

    // Set handle colors
    qint32 size = this->resize_handles_->size();
    qint32 index = 0;
    for (PolygonResizeHandle *handle : *this->resize_handles_) {
        QColor color = Qt::white;
//        if (index == 0) {
//            color = Qt::green;
//        } else
        if (index == size - 1) {
            color = Qt::red;
        }
        handle->setColor(color);
        handle->updatePos();
        index++;
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
    for (QPointF *point : *this->waypoints_) {
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

}  // namespace interface
