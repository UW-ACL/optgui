// TITLE:   Optimization_Interface/plane_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "plane_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineF>

#include "globals.h"

namespace interface {

PlaneGraphicsItem::PlaneGraphicsItem(PlaneModelItem *model,
                                     QGraphicsItem *parent)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;
    this->initialize();
}

void PlaneGraphicsItem::initialize() {
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
    this->p1_handle_ = new PolygonResizeHandle(this->model_->p1_, this);
    this->p2_handle_ = new PolygonResizeHandle(this->model_->p2_, this);
    this->p1_handle_->hide();
    this->p2_handle_->hide();
}

PlaneGraphicsItem::~PlaneGraphicsItem() {
    delete this->p1_handle_;
    delete this->p2_handle_;
}

QRectF PlaneGraphicsItem::boundingRect() const {
    return this->shape().boundingRect();
}

void PlaneGraphicsItem::paint(QPainter *painter,
                              const QStyleOptionGraphicsItem *option,
                              QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Show handles if selected
    if (this->isSelected()) {
        this->p1_handle_->updatePos();
        this->p2_handle_->updatePos();
        this->p1_handle_->show();
        this->p2_handle_->show();

        this->pen_.setWidth(3);
    } else {
        this->p1_handle_->hide();
        this->p2_handle_->hide();

        this->pen_.setWidth(1);
    }

    // Draw shape
    painter->setPen(this->pen_);
    painter->setBrush(this->brush_);
    painter->fillPath(this->shape(), this->brush_);
    QLineF line(mapFromScene(*this->model_->p1_),
                mapFromScene(*this->model_->p2_));
    painter->drawLine(line);
}

int PlaneGraphicsItem::type() const {
    return PLANE_GRAPHIC;
}

QPainterPath PlaneGraphicsItem::shape() const {
    QPainterPath path;

    QLineF line(mapFromScene(*this->model_->p1_),
                mapFromScene(*this->model_->p2_));
    // Flip shaded side if direction
    if (this->model_->direction_) {
        line = QLineF(line.p2(), line.p1());
    }

    QPolygonF poly;
    poly << line.p1();
    poly << line.p2();
    poly << line.normalVector().translated(
                line.dx(), line.dy()).pointAt(PLANE_BORDER / line.length());
    poly << line.normalVector().pointAt(PLANE_BORDER / line.length());
    path.addPolygon(poly);

    return path;
}

void PlaneGraphicsItem::expandScene() {
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

void PlaneGraphicsItem::flipDirection() {
    this->model_->direction_ = !this->model_->direction_;
    this->expandScene();
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

        // check to expand the scene
        this->expandScene();
    }
    return QGraphicsItem::itemChange(change, value);
}

}  // namespace interface
