// TITLE:   Optimization_Interface/path_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "path_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineF>

#include "globals.h"

namespace interface {

PathGraphicsItem::PathGraphicsItem(QPointF *point, QVector<QPointF *> *path,
                                   QGraphicsItem *parent)
    : QGraphicsItem(parent) {
    // Set model
    this->point_ = point;
    this->path_ = path;
    this->initialize();
}

void PathGraphicsItem::initialize() {
    // Set pen
    this->pen_ = QPen(Qt::black);
    this->pen_.setWidth(3);

    // Set flags
    this->setFlags(QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemSendsGeometryChanges);

    // Set position
    this->setPos(this->point_->x(), this->point_->y());
}

QRectF PathGraphicsItem::boundingRect() const {
    return this->shape().boundingRect();
}

void PathGraphicsItem::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *option,
                             QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (this->isSelected()) {
        this->pen_.setWidth(3);
    } else {
        this->pen_.setWidth(1);
    }
    painter->setPen(this->pen_);

    if (this->point_ == this->path_->first()) {
        // Draw connecting path
        painter->setPen(Qt::blue);
        for (qint32 i = 1; i < this->path_->length(); i++) {
            QLineF line(mapFromScene(*this->path_->at(i-1)),
                        mapFromScene(*this->path_->at(i)));
            painter->drawLine(line);
        }

        painter->setPen(this->pen_);
        painter->setBrush(Qt::green);
    } else if (this->point_ == this->path_->last()) {
        painter->setBrush(Qt::red);
    } else {
        painter->setBrush(Qt::white);
    }

    painter->drawPath(this->shape());
}

int PathGraphicsItem::type() const {
    return PATH_GRAPHIC;
}

QPainterPath PathGraphicsItem::shape() const {
    QPainterPath path;
    path.addEllipse(QRectF(-PATH_POINT_SIZE / 2, -PATH_POINT_SIZE / 2,
                           PATH_POINT_SIZE, PATH_POINT_SIZE));
    return path;
}

void PathGraphicsItem::expandScene() {
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

QVariant PathGraphicsItem::itemChange(GraphicsItemChange change,
                                      const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();

        // update model
        this->point_->setX(newPos.x());
        this->point_->setY(newPos.y());

        // check to expand the scene
        this->expandScene();
    }
    return QGraphicsItem::itemChange(change, value);
}

}  // namespace interface
