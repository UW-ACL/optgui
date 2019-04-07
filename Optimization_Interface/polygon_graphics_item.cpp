// TITLE:   Optimization_Interface/polygon_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "polygon_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineF>

#include "globals.h"

namespace interface {

PolygonGraphicsItem::PolygonGraphicsItem(PolygonModelItem *model,
                                         QGraphicsItem *parent)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;
    this->initialize();
}

void PolygonGraphicsItem::initialize() {
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
    this->resize_handles_ = new QVector<PolygonResizeHandle *>();
    for (QPointF *point : *this->model_->points_) {
        PolygonResizeHandle *handle = new PolygonResizeHandle(point, this);
        this->resize_handles_->append(handle);
        handle->hide();
    }
}

PolygonGraphicsItem::~PolygonGraphicsItem() {
    // Delete resize handles
    for (PolygonResizeHandle *handle : *this->resize_handles_) {
        delete handle;
    }
    delete this->resize_handles_;
}

QRectF PolygonGraphicsItem::boundingRect() const {
    return this->shape().boundingRect();
}

void PolygonGraphicsItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Show handles if selected
    if (this->isSelected()) {
        for (PolygonResizeHandle *handle : *this->resize_handles_) {
            handle->updatePos();
            handle->show();
        }
        this->pen_.setWidth(3);
    } else {
        for (PolygonResizeHandle *handle : *this->resize_handles_) {
            handle->hide();
        }
        this->pen_.setWidth(1);
    }

    painter->setPen(this->pen_);
    painter->setBrush(this->brush_);

    painter->fillPath(this->shape(), this->brush_);
    for (qint32 i = 1; i < this->model_->points_->length(); i++) {
        QLineF line(mapFromScene(*this->model_->points_->at(i-1)),
                    mapFromScene(*this->model_->points_->at(i)));
        painter->drawLine(line);
    }
    QLineF line(mapFromScene(*this->model_->points_->last()),
                mapFromScene(*this->model_->points_->first()));
    painter->drawLine(line);
}

int PolygonGraphicsItem::type() const {
    return POLYGON_GRAPHIC;
}

QPainterPath PolygonGraphicsItem::shape() const {
    QPainterPath path;

    // Draw exterior shadings
    for (qint32 i = 1; i < this->model_->points_->length(); i++) {
        QLineF line(mapFromScene(*this->model_->points_->at(i-1)),
                    mapFromScene(*this->model_->points_->at(i)));
        // Flip shading if direction is reversed
        if (this->model_->direction_) {
            line = QLineF(line.p2(), line.p1());
        }
        QPolygonF poly;
        poly << line.p1();
        poly << line.p2();
        poly << line.normalVector().translated(
                    line.dx(),
                    line.dy()).pointAt(POLYGON_BORDER /line.length());
        poly << line.normalVector().pointAt(POLYGON_BORDER / line.length());
        path.addPolygon(poly);
    }
    QLineF line(mapFromScene(*this->model_->points_->last()),
                mapFromScene(*this->model_->points_->first()));
    // Flip shading if direction is reversed
    if (this->model_->direction_) {
        line = QLineF(line.p2(), line.p1());
    }

    QPolygonF poly;
    poly << line.p1();
    poly << line.p2();
    poly << line.normalVector().translated(
                line.dx(), line.dy()).pointAt(POLYGON_BORDER / line.length());
    poly << line.normalVector().pointAt(POLYGON_BORDER / line.length());
    path.addPolygon(poly);

    return path;
}

void PolygonGraphicsItem::expandScene() {
    if (scene()) {
        // expand scene if item goes out of bounds
        QRectF newRect = this->sceneBoundingRect();
        QRectF rect = this->scene()->sceneRect();
        if (!rect.contains(newRect)) {
            this->scene()->setSceneRect(
                        this->scene()->sceneRect().united(newRect));
            if (!this->scene()->views().isEmpty()) {
                this->scene()->views().first()->setSceneRect(
                            this->scene()->sceneRect());
            }
        }
        this->scene()->update();
    }
}

void PolygonGraphicsItem::flipDirection() {
    this->model_->direction_ = !this->model_->direction_;
    this->expandScene();
}

QVariant PolygonGraphicsItem::itemChange(GraphicsItemChange change,
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
