// TITLE:   Optimization_Interface/src/graphics/ellipse_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/ellipse_graphics_item.h"

#include <QGraphicsScene>
#include <QtMath>
#include <QGraphicsView>

#include "include/globals.h"

namespace interface {

EllipseGraphicsItem::EllipseGraphicsItem(EllipseModelItem *model,
                                         QGraphicsItem *parent)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;
    this->initialize();
}

void EllipseGraphicsItem::initialize() {
    // Set pen
    QColor fill = Qt::gray;
    fill.setAlpha(200);
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

    // Set resize handle
    this->resize_handle_ =
            new EllipseResizeHandle(this->model_, this);
    this->resize_handle_->hide();
}

EllipseGraphicsItem::~EllipseGraphicsItem() {
    delete this->resize_handle_;
}

QRectF EllipseGraphicsItem::boundingRect() const {
    qreal rad = this->model_->radius_;
    // Add exterior border if not direction
    if (!this->model_->direction_) {
        // scale with view
        rad += ELLIPSE_BORDER / this->getScalingFactor();
    }
    return QRectF(-rad, -rad, rad * 2, rad * 2);
}

void EllipseGraphicsItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    qreal scaling_factor = this->getScalingFactor();

    this->setPos(*this->model_->pos_);

    // Show handles if selected
    if (this->isSelected()) {
        this->resize_handle_->setPos(-this->model_->radius_, 0);
        this->resize_handle_->show();

        this->pen_.setWidthF(3.0 / scaling_factor);
    } else {
        this->resize_handle_->hide();

        this->pen_.setWidthF(1.0 / scaling_factor);
    }

    painter->setPen(this->pen_);

    // Draw shape
    painter->fillPath(this->shape(), this->brush_);
    double rad = this->model_->radius_;
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

int EllipseGraphicsItem::type() const {
    return ELLIPSE_GRAPHIC;
}

QPainterPath EllipseGraphicsItem::shape() const {
    QPainterPath path;
    path.addEllipse(this->boundingRect());
    // Add interior border if not direction
    if (!this->model_->direction_) {
        double rad = this->model_->radius_;
        path.addEllipse(QRectF(-rad, -rad, rad * 2, rad * 2));
    }
    return path;
}

void EllipseGraphicsItem::expandScene() {
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
        this->update(this->boundingRect());
    }
}

void EllipseGraphicsItem::flipDirection() {
    this->model_->direction_ = !this->model_->direction_;
    this->expandScene();
}

QVariant EllipseGraphicsItem::itemChange(GraphicsItemChange change,
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

qreal EllipseGraphicsItem::getScalingFactor() const {
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace interface
