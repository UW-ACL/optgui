// TITLE:   Optimization_Interface/src/graphics/polygon_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/polygon_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineF>

#include "include/globals.h"

namespace optgui {

PolygonGraphicsItem::PolygonGraphicsItem(PolygonModelItem *model,
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

    // Set flags
    this->setFlags(QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemSendsGeometryChanges);

    // Set resize handles
    quint32 size = this->model_->getSize();
    for (quint32 i = 0; i < size; i++) {
        PolygonResizeHandle *handle =
                new PolygonResizeHandle(this->model_, i, this);
        this->resize_handles_.append(handle);
        handle->hide();
    }
}

PolygonGraphicsItem::~PolygonGraphicsItem() {
    // Delete resize handles
    for (PolygonResizeHandle *handle : this->resize_handles_) {
        delete handle;
    }
}

QRectF PolygonGraphicsItem::boundingRect() const {
    return this->shape().boundingRect();
}

void PolygonGraphicsItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Change color based on convexity
    QColor fill = Qt::gray;
    /*
    if (this->model_->isConvex()) {
        fill = Qt::gray;
    } else {
        fill = RED;
    }
    */
    fill.setAlpha(200);
    this->brush_ = QBrush(fill);

    // scale with view
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }

    // Show handles if selected
    if (this->isSelected()) {
        for (PolygonResizeHandle *handle : this->resize_handles_) {
            handle->updatePos();
            handle->show();
        }

        this->pen_.setWidthF(3.0 / scaling_factor);
    } else {
        for (PolygonResizeHandle *handle : this->resize_handles_) {
            handle->hide();
        }

        this->pen_.setWidthF(1.0 / scaling_factor);
    }

    painter->setPen(this->pen_);
    painter->setBrush(this->brush_);


    // Fill shading
    painter->fillPath(this->shape(), this->brush_);

    // Draw outline
    quint32 size = this->model_->getSize();
    for (quint32 i = 1; i < size + 1; i++) {
        QLineF line(mapFromScene(this->model_->getPointAt(i-1)),
                    mapFromScene(this->model_->getPointAt(i % size)));
        painter->drawLine(line);
    }

    // Label with port
    if (this->model_->port_ != 0) {
        QPointF text_pos(this->mapFromScene(this->model_->getPointAt(0)));
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

int PolygonGraphicsItem::type() const {
    return POLYGON_GRAPHIC;
}

QPainterPath PolygonGraphicsItem::shape() const {
    QPainterPath path;

    // scale border with view
    qreal border = POLYGON_BORDER / this->getScalingFactor();

    // Define exterior shadings
    quint32 size = this->model_->getSize();
    for (quint32 i = 1; i < size + 1; i++) {
        QLineF line(mapFromScene(this->model_->getPointAt(i - 1)),
                    mapFromScene(this->model_->getPointAt(i % size)));
        // Flip shading if direction is reversed
        if (this->model_->getDirection()) {
            line = QLineF(line.p2(), line.p1());
        }
        QPolygonF poly;
        poly << line.p1();
        poly << line.p2();
        poly << line.normalVector().translated(
                    line.dx(),
                    line.dy()).pointAt(border / line.length());
        poly << line.normalVector().pointAt(border / line.length());
        path.addPolygon(poly);
    }

    // Return shape
    return path;
}

void PolygonGraphicsItem::flipDirection() {
    this->model_->flipDirection();
    this->update(this->boundingRect());
}

QVariant PolygonGraphicsItem::itemChange(GraphicsItemChange change,
                                         const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();

        // update model
        QPointF diff = newPos - this->scenePos();
        for (PolygonResizeHandle *handle : this->resize_handles_) {
            handle->updateModel(diff);
        }

        // check to expand the scene
        this->update(this->boundingRect());
    }
    return QGraphicsItem::itemChange(change, value);
}

qreal PolygonGraphicsItem::getScalingFactor() const {
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

}  // namespace optgui
