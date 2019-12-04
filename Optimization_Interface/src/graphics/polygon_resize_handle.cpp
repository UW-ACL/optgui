// TITLE:   Optimization_Interface/src/graphics/polygon_resize_handle.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "../../include/graphics/polygon_resize_handle.h"

#include <QtMath>
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "../../include/globals.h"

namespace interface {

PolygonResizeHandle::PolygonResizeHandle(QPointF *point,
                                         QGraphicsItem *parent,
                                         qreal size)
    : QGraphicsEllipseItem(parent) {
    this->point_ = point;
    this->resize_ = false;
    this->setPen(QPen(Qt::black));
    this->setBrush(QBrush(Qt::white));
    this->size_ = size;
    this->index_ = 0;
    this->setRect(-this->size_, -this->size_,
                  this->size_ * 2, this->size_ * 2);
}

void PolygonResizeHandle::setColor(const QColor color) {
    this->setBrush(QBrush(color));
}

void PolygonResizeHandle::updatePos() {
    // Translate model point to local coordinates
    this->setPos(this->parentItem()->mapFromScene(*this->point_));
}

void PolygonResizeHandle::updateModel(QPointF diff) {
    // Adjust model to new parent position
    *this->point_ += diff;
}

void PolygonResizeHandle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->resize_ = true;
    }
}

void PolygonResizeHandle::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // scale with view
    qreal scaling_factor = this->getScalingFactor();
    qreal size = this->size_ / scaling_factor;
    QPen pen = this->pen();
    pen.setWidthF(1.0 / scaling_factor);
    this->setPen(pen);
    this->setRect(-size, -size, size * 2, size * 2);

    // paint
    QGraphicsEllipseItem::paint(painter, option, widget);

    if (this->index_ > 0) {
        painter->setPen(Qt::black);
        QFont font = painter->font();
        font.setPointSizeF(14 / scaling_factor);
        painter->setFont(font);
        painter->drawText(this->boundingRect(), Qt::AlignCenter,
                          QString::number(this->index_));
    }
}

void PolygonResizeHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->resize_ = false;
    }
}

void PolygonResizeHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (this->resize_) {
        QPointF eventPos = event->scenePos();
        this->point_->setX(eventPos.x());
        this->point_->setY(eventPos.y());
        this->expandScene();
    }
}

int PolygonResizeHandle::type() const {
    return HANDLE_GRAPHIC;
}

QPointF *PolygonResizeHandle::getPoint() {
    return this->point_;
}

void PolygonResizeHandle::expandScene() {
    if (scene()) {
        // expand scene if item goes out of bounds
        QRectF newRect = this->parentItem()->sceneBoundingRect();
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

qreal PolygonResizeHandle::getScalingFactor() {
    qreal scaling_factor = 1;
    if (this->scene() && !this->scene()->views().isEmpty()) {
        scaling_factor = this->scene()->views().first()->matrix().m11();
    }
    return scaling_factor;
}

void PolygonResizeHandle::setIndex(quint32 index) {
    this->index_ = index;
}

}  // namespace interface
