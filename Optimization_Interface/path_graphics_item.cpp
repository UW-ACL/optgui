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

PathGraphicsItem::PathGraphicsItem(QVector<QPointF *> *path,
                                   QGraphicsItem *parent)
    : QGraphicsItem(parent) {
    // Set model
    this->path_ = path;
    this->initialize();
}

void PathGraphicsItem::initialize() {
    // Set pen
    this->pen_ = QPen(Qt::blue);
    this->pen_.setWidth(3);

    // Set flags
    this->setFlags(QGraphicsItem::ItemSendsGeometryChanges);

    // Initialize resize handles
    this->resize_handles_ = new QVector<PolygonResizeHandle *>();
}

PathGraphicsItem::~PathGraphicsItem() {
    // Delete resize handles
    for (PolygonResizeHandle *handle : *this->resize_handles_) {
        delete handle;
    }
    delete this->resize_handles_;
}

QRectF PathGraphicsItem::boundingRect() const {
    return this->shape().boundingRect();
}

void PathGraphicsItem::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *option,
                             QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Create new handles if necessary
    if (this->path_->size() > this->resize_handles_->size()) {
        for (qint32 i = this->resize_handles_->size(); i < this->path_->size(); i++) {
            PolygonResizeHandle *handle = new PolygonResizeHandle(this->path_->at(i), this);
            this->resize_handles_->append(handle);
            handle->show();
        }
    }

    // Draw connecting path
    painter->setPen(this->pen_);
    for (qint32 i = 1; i < this->path_->length(); i++) {
        QLineF line(mapFromScene(*this->path_->at(i-1)),
                    mapFromScene(*this->path_->at(i)));
        painter->drawLine(line);
    }

    // Set handle colors
    qint32 size = this->resize_handles_->size();
    qint32 index = 0;
    for (PolygonResizeHandle *handle : *this->resize_handles_) {
        QColor color = Qt::white;
        if (index == 0) {
            color = Qt::green;
        } else if (index == size - 1) {
            color = Qt::red;
        }
        handle->setColor(color);
        handle->updatePos();
        index++;
    }
}

void PathGraphicsItem::removeHandle(PolygonResizeHandle *handle) {
    this->resize_handles_->removeOne(handle);
}

int PathGraphicsItem::type() const {
    return PATH_GRAPHIC;
}

QPainterPath PathGraphicsItem::shape() const {
    QPainterPath path;
    QPolygonF poly;
    for (QPointF *point : *this->path_) {
        poly << *point;
    }
    path.addPolygon(poly);
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
        QPointF diff = newPos - this->scenePos();
        for (PolygonResizeHandle *handle : *this->resize_handles_) {
            handle->updateModel(diff);
        }

        // check to expand the scene
        this->expandScene();
    }
    return QGraphicsItem::itemChange(change, value);
}

}  // namespace interface
