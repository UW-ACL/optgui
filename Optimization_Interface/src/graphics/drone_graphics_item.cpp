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
    this->setFlags(QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemSendsGeometryChanges);

    // set radius
    this->size_ = size;
    this->is_curr_drone_ = false;
    this->is_staged_drone_ = false;
    this->is_executed_drone_ = false;

    // Set position
    QVector3D pos_3D = this->model_->getPos();
    QPointF pos_2D = QPointF(pos_3D.x(), pos_3D.y());
    this->setPos(pos_2D);
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
    QVector3D pos_3D = this->model_->getPos();
    QPointF pos_2D = QPointF(pos_3D.x(), pos_3D.y());
    this->setPos(pos_2D);

    // scale with view zoom level
    qreal scaling_factor = this->getScalingFactor();
    if (this->isSelected()) {
        this->pen_.setWidthF(3.0 / scaling_factor);
    } else {
        this->pen_.setWidthF(1.0 / scaling_factor);
    }

    // draw curr selected drone circle
    if (this->is_curr_drone_
            || this->is_staged_drone_
            || this->is_executed_drone_) {
        QPen selection_pen(YELLOW);
        if (this->is_executed_drone_) {
            selection_pen = QPen(CYAN);
        } else if (this->is_staged_drone_) {
            selection_pen = QPen(GREEN);
        }
        selection_pen.setWidthF(3.0 / scaling_factor);
        painter->setPen(selection_pen);
        painter->drawEllipse(QPointF(),
                             this->size_ / scaling_factor,
                             this->size_ / scaling_factor);
    }

    // Draw drone
    painter->setPen(this->pen_);
    painter->setBrush(this->brush_);

    painter->drawPath(this->shape());

    // Label with port
    if (this->model_->port_ != 0) {
        QPointF text_pos(this->mapFromScene(pos_2D));
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

int DroneGraphicsItem::type() const {
    return DRONE_GRAPHIC;
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
    if (change == ItemPositionChange && this->scene()) {
        // value is the new position
        QPointF newPos = value.toPointF();

        // update model
        this->model_->setPos(QVector3D(newPos.x(), newPos.y(), 0));

        // check to expand the scene
        this->update(this->boundingRect());
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

}  // namespace optgui
