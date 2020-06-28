// TITLE:   Optimization_Interface/src/graphics/point_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/point_graphics_item.h"

#include <QGraphicsScene>
#include <QtMath>
#include <QGraphicsView>

#include "include/globals.h"

namespace optgui {

PointGraphicsItem::PointGraphicsItem(PointModelItem *model,
                                     QGraphicsItem *parent,
                                     qreal radius)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;
    this->radius_ = radius;

    // Set pen
    QColor fill = RED;
    this->brush_ = QBrush(fill);

    // Set brush
    this->pen_ = QPen(Qt::black);
    this->pen_.setWidth(3);

    // Set flags
    this->setFlags(QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemSendsGeometryChanges);

    // Set position
    this->setPos(this->model_->getPos());
}

QRectF PointGraphicsItem::boundingRect() const {
    // return area of point scaled by zoom factor
    qreal scaling_factor = this->getScalingFactor();
    qreal rad = this->radius_ / scaling_factor;
    return QRectF(-rad, -rad, rad * 2, rad * 2);
}

void PointGraphicsItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget) {
    // suppress unused options error
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // scale with view
    qreal scaling_factor = this->getScalingFactor();

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

    // Label with port
    if (this->model_->port_ != 0) {
        painter->setPen(BLACK);
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

int PointGraphicsItem::type() const {
    // return unique graphics type
    return POINT_GRAPHIC;
}

QPainterPath PointGraphicsItem::shape() const {
    // return shape of point
    QPainterPath path;
    path.addEllipse(this->boundingRect());
    return path;
}

QVariant PointGraphicsItem::itemChange(GraphicsItemChange change,
                                       const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();

        // update model
        this->model_->setPos(newPos);

        // check to redraw
        this->update(this->boundingRect());
    }
    return QGraphicsItem::itemChange(change, value);
}

qreal PointGraphicsItem::getScalingFactor() const {
    // get zoom scaling factor from view
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace optgui
