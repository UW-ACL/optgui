// TITLE:   Optimization_Interface/src/graphics/plane_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/plane_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineF>

#include "include/globals.h"

namespace optgui {

PlaneGraphicsItem::PlaneGraphicsItem(PlaneModelItem *model,
                                     QGraphicsItem *parent)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;

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

    // Set resize handles
    this->p1_handle_ =
            new PlaneResizeHandle(this->model_, false, this);
    this->p2_handle_ =
            new PlaneResizeHandle(this->model_, true, this);
    //
    this->p1_handle_->hide();
    this->p2_handle_->hide();
}

PlaneGraphicsItem::~PlaneGraphicsItem() {
    // clean up resize handles
    delete this->p1_handle_;
    delete this->p2_handle_;
}

QRectF PlaneGraphicsItem::boundingRect() const {
    return this->shape().boundingRect();
}

void PlaneGraphicsItem::paint(QPainter *painter,
                              const QStyleOptionGraphicsItem *option,
                              QWidget *widget) {
    // suppress unused options errors
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // scale with view
    qreal scaling_factor = this->getScalingFactor();

    // Show handles if selected
    if (this->isSelected()) {
        this->p1_handle_->updatePos();
        this->p2_handle_->updatePos();
        this->p1_handle_->show();
        this->p2_handle_->show();

        this->pen_.setWidthF(3.0 / scaling_factor);
    } else {
        this->p1_handle_->hide();
        this->p2_handle_->hide();

        this->pen_.setWidthF(1.0 / scaling_factor);
    }

    // Draw shape
    painter->setPen(this->pen_);
    painter->setBrush(this->brush_);
    painter->fillPath(this->shape(), this->brush_);
    QLineF line(mapFromScene(this->model_->getP1()),
                mapFromScene(this->model_->getP2()));
    painter->drawLine(line);

    // Label with port
    if (this->model_->port_ != 0) {
        QPointF text_pos(this->mapFromScene(this->model_->getP1()));
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

int PlaneGraphicsItem::type() const {
    return PLANE_GRAPHIC;
}

QPainterPath PlaneGraphicsItem::shape() const {
    QPainterPath path;

    QLineF line(mapFromScene(this->model_->getP1()),
                mapFromScene(this->model_->getP2()));
    // Flip shaded side if direction
    if (this->model_->getDirection()) {
        line = QLineF(line.p2(), line.p1());
    }

    // scale border with view
    qreal border = PLANE_BORDER / this->getScalingFactor();

    QPolygonF poly;
    poly << line.p1();
    poly << line.p2();
    poly << line.normalVector().translated(
                line.dx(), line.dy()).pointAt(border / line.length());
    poly << line.normalVector().pointAt(border / line.length());
    path.addPolygon(poly);

    return path;
}

void PlaneGraphicsItem::flipDirection() {
    this->model_->flipDirection();
    this->update(this->boundingRect());
}

QVariant PlaneGraphicsItem::itemChange(GraphicsItemChange change,
                                       const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();

        // Update model
        QPointF diff = newPos - this->scenePos();
        this->p1_handle_->updateModel(diff);
        this->p2_handle_->updateModel(diff);

        // check to redraw
        this->update(this->boundingRect());
    }
    return QGraphicsItem::itemChange(change, value);
}

qreal PlaneGraphicsItem::getScalingFactor() const {
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace optgui
