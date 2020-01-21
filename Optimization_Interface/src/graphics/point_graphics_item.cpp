// TITLE:   Optimization_Interface/src/graphics/point_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/point_graphics_item.h"

#include <QGraphicsScene>
#include <QtMath>
#include <QGraphicsView>

#include "include/globals.h"

namespace interface {

PointGraphicsItem::PointGraphicsItem(PointModelItem *model,
                                     QGraphicsItem *parent,
                                     qreal size)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;
    this->radius_ = size;
    this->initialize();
}

void PointGraphicsItem::initialize() {
    // Set pen
    QColor fill = Qt::red;
    this->brush_ = QBrush(fill);

    // Set brush
    this->pen_ = QPen(Qt::black);
    this->pen_.setWidth(3);

    // Set flags
    this->setFlags(QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemSendsGeometryChanges);

    // Set position
    this->setPos(*this->model_->pos_);
}

QRectF PointGraphicsItem::boundingRect() const {
    qreal scaling_factor = this->getScalingFactor();
    qreal rad = this->radius_ / scaling_factor;
    return QRectF(-rad, -rad, rad * 2, rad * 2);
}

void PointGraphicsItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // scale with view
    qreal scaling_factor = this->getScalingFactor();

    this->setPos(*this->model_->pos_);

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
        QPointF text_pos(this->mapFromScene(*this->model_->pos_));
        QFont font = painter->font();
        font.setPointSizeF(12 / scaling_factor);
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
    return POINT_GRAPHIC;
}

QPainterPath PointGraphicsItem::shape() const {
    QPainterPath path;
    path.addEllipse(this->boundingRect());
    return path;
}

void PointGraphicsItem::expandScene() {
    if (scene()) {
        // expand scene if item goes out of bounds
        QRectF newRect = this->sceneBoundingRect();
        QRectF rect = this->scene()->sceneRect();
        if (!rect.contains(newRect)) {
            this->scene()->setSceneRect(scene()->sceneRect().united(newRect));

            if (!this->scene()->views().isEmpty()) {
                this->scene()->views().first()->setSceneRect(
                            this->scene()->sceneRect());
            }
        }
        this->scene()->update();
    }
}

QVariant PointGraphicsItem::itemChange(GraphicsItemChange change,
                                       const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();

        // update model
        *this->model_->pos_ = newPos;

        // check to expand the scene
        this->expandScene();
    }
    return QGraphicsItem::itemChange(change, value);
}

qreal PointGraphicsItem::getScalingFactor() const {
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace interface
