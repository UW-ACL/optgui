// TITLE:   Optimization_Interface/src/graphics/waypoint_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/waypoint_graphics_item.h"

#include <QtMath>
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "include/globals.h"

namespace optgui {

WaypointGraphicsItem::WaypointGraphicsItem(PointModelItem *model,
                                         quint32 index,
                                         QGraphicsItem *parent,
                                         qreal radius)
    : QGraphicsItem(parent) {
    // set data model
    this->model_ = model;

    // set graphical info
    this->pen_ = QPen(Qt::black);
    this->pen_.setWidthF(3);
    this->brush_ = QBrush(Qt::white);

    // set size of waypoint
    this->radius_ = radius;

    // set ordering of waypoint
    this->index_ = index;

    // Set flags
    this->setFlags(QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemSendsGeometryChanges);

    // Set position
    this->setPos(this->model_->getPos());
}

QRectF WaypointGraphicsItem::boundingRect() const {
    // get rough area circle scaled by zoom factor
    qreal scaling_factor = this->getScalingFactor();
    qreal rad = this->radius_ / scaling_factor;
    return QRectF(-rad, -rad, rad * 2, rad * 2);
}

void WaypointGraphicsItem::paint(QPainter *painter,
                              const QStyleOptionGraphicsItem *option,
                              QWidget *widget) {
    // suppress unused options error
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // scale with view
    qreal scaling_factor = this->getScalingFactor();

    // update graphic with pos from data model
    this->setPos(this->model_->getPos());

    // Show handles if selected
    if (this->isSelected()) {
        this->pen_.setWidthF(3.0 / scaling_factor);
    } else {
        this->pen_.setWidthF(1.0 / scaling_factor);
    }

    painter->setPen(this->pen_);

    // Draw shape
    painter->fillPath(this->shape(), this->brush_);
    qreal rad = this->radius_ / scaling_factor;
    painter->drawEllipse(QRectF(-rad, -rad, rad * 2, rad * 2));

    // Draw label
    painter->setPen(BLACK);
    QFont font = painter->font();

    if (this->model_->port_ == 0) {
        // label with index
        font.setPointSizeF(14 / scaling_factor);
        painter->setFont(font);
        painter->drawText(this->boundingRect(), Qt::AlignCenter,
                          QString::number(this->index_ + 1));
    } else {
        // Or label with port
        QPointF text_pos(this->mapFromScene(this->model_->getPos()));
        QFont font = painter->font();
        font.setPointSizeF(10 / scaling_factor);
        painter->setFont(font);
        qreal text_box_size = 50.0 / scaling_factor;
        painter->drawText(text_pos.x() - text_box_size,
                          text_pos.y() - text_box_size,
                          text_box_size * 2, text_box_size * 2,
                          Qt::AlignCenter,
                          QString::number(this->model_->port_));
    }
}

void WaypointGraphicsItem::setIndex(quint32 index) {
    // set ordering of waypoint
    this->index_ = index;
}

QPainterPath WaypointGraphicsItem::shape() const {
    // get shape of circle to draw
    QPainterPath path;
    path.addEllipse(this->boundingRect());
    return path;
}

int WaypointGraphicsItem::type() const {
    // get unique graphics type
    return WAYPOINT_GRAPHIC;
}

QVariant WaypointGraphicsItem::itemChange(GraphicsItemChange change,
                                       const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();

        // update model
        this->model_->setPos(newPos);

        // redraw graphic
        this->update(this->boundingRect());
    }
    return QGraphicsItem::itemChange(change, value);
}

qreal WaypointGraphicsItem::getScalingFactor() const {
    // get zoom scaling factor from view
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace optgui
