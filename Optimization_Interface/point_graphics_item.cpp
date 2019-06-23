// TITLE:   Optimization_Interface/point_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "point_graphics_item.h"

#include <QGraphicsScene>
#include <QtMath>
#include <QGraphicsView>

#include "globals.h"

namespace interface {

PointGraphicsItem::PointGraphicsItem(PointModelItem *model,
                                     QGraphicsItem *parent,
                                     quint32 size)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;
    this->radius_ = size;
    this->initialize();
}

void PointGraphicsItem::initialize() {
    // Set pen
    QColor fill = Qt::red;
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
    this->setPos(this->mapFromScene(*this->model_->pos_));

    this->marker_ = false;
}

PointGraphicsItem::~PointGraphicsItem() {
}

QRectF PointGraphicsItem::boundingRect() const {
    double rad = this->radius_;
    // Add exterior border if not direction
    if (!this->model_->direction_) {
        rad += POINT_BORDER;
    }
    return QRectF(-rad, -rad, rad * 2, rad * 2);
}

void PointGraphicsItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    // TODO: fix this, use mapfromscene
    this->setPos(*this->model_->pos_);//this->mapFromScene(*this->model_->pos_));

    // Show handles if selected
    if (this->isSelected()) {
        this->pen_.setWidth(3);
    } else {
        this->pen_.setWidth(1);
    }

    painter->setPen(this->pen_);

    // Draw shape
    painter->fillPath(this->shape(), this->brush_);

    double rad = this->radius_;
    switch (this->marker_) {
    case 0:
        painter->drawEllipse(QRectF(-rad, -rad, rad * 2, rad * 2));
        break;
    case 1:
        painter->setPen(QPen(Qt::green, this->radius_*2));
        painter->drawLine(-rad*5,-rad*5,rad*5,rad*5);
        painter->drawLine(rad*5,-rad*5,-rad*5,rad*5);
        break;
    }

    // Label with port
    if (this->model_->port_ != 0) {
        QPointF text_pos(this->mapFromScene(*this->model_->pos_));
        painter->drawText(QRectF(text_pos.x(), text_pos.y(), 50, 15),
                          QString::number(this->model_->port_));
    }
}

void PointGraphicsItem::setMarker(uint32_t type) {
    this->marker_ = type;
}

int PointGraphicsItem::type() const {
    return POINT_GRAPHIC;
}

QPainterPath PointGraphicsItem::shape() const {
    QPainterPath path;
    path.addEllipse(this->boundingRect());
    // Add exterior border if not direction
    if (!this->model_->direction_) {
        double rad = 0.03;
        path.addEllipse(QRectF(-rad, -rad, rad * 2, rad * 2));
    }
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

}  // namespace interface
