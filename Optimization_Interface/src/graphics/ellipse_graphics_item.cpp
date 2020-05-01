// TITLE:   Optimization_Interface/src/graphics/ellipse_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/ellipse_graphics_item.h"

#include <QGraphicsScene>
#include <QtMath>
#include <QGraphicsView>

#include "include/globals.h"

namespace optgui {

EllipseGraphicsItem::EllipseGraphicsItem(EllipseModelItem *model,
                                         QGraphicsItem *parent)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;
    this->initialize();
}

void EllipseGraphicsItem::initialize() {
    // Set brush
    QColor fill = Qt::gray;
    fill.setAlpha(200);
    this->brush_ = QBrush(fill);

    // Set pen
    this->pen_ = QPen(Qt::black);
    this->pen_.setWidth(3);

    // Set clearance pen
    this->clearance_pen_ = QPen(fill, 3, Qt::DashLine);

    // Set flags
    this->setFlags(QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemSendsGeometryChanges);

    // Set position
    this->setPos(this->model_->getPos());

    // Set resize handles
    this->width_handle_ =
            new EllipseResizeHandle(this->model_, this, 0);
    this->height_handle_ =
            new EllipseResizeHandle(this->model_, this, 1);
    this->radius_handle_ =
            new EllipseResizeHandle(this->model_, this, 2);
    this->width_handle_->hide();
    this->height_handle_->hide();
    this->radius_handle_->hide();

    // set granularity for collision detection
    // collision detection is iterative, value between
    // 0 and 1 balances precision with performance
    this->setBoundingRegionGranularity(0.9);
}

EllipseGraphicsItem::~EllipseGraphicsItem() {
    delete this->width_handle_;
    delete this->height_handle_;
    delete this->radius_handle_;
}

QRectF EllipseGraphicsItem::boundingRect() const {
    qreal height = this->model_->getHeight() +
            (this->model_->getClearance() * GRID_SIZE);
    qreal width = this->model_->getWidth() +
            (this->model_->getClearance() * GRID_SIZE);
    // Add exterior border if direction flipped
    if (this->model_->getDirection()) {
        // scale with view
        height += ELLIPSE_BORDER / this->getScalingFactor();
        width += ELLIPSE_BORDER / this->getScalingFactor();
    }
    return QRectF(-width, -height, width * 2, height * 2);
}

void EllipseGraphicsItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    qreal scaling_factor = this->getScalingFactor();
    qreal width = this->model_->getWidth();
    qreal height = this->model_->getHeight();
    QPointF pos = this->model_->getPos();

    this->setPos(pos);

    // Show handles if selected
    if (this->isSelected()) {
        this->width_handle_->setPos(-width, 0);
        this->height_handle_->setPos(0, -height);
        this->radius_handle_->setPos(-width * qCos(qDegreesToRadians(45.0)),
                                     -height * qSin(qDegreesToRadians(45.0)));

        this->width_handle_->show();
        this->height_handle_->show();
        this->radius_handle_->show();

        this->pen_.setWidthF(3.0 / scaling_factor);
    } else {
        this->width_handle_->hide();
        this->height_handle_->hide();
        this->radius_handle_->hide();

        this->pen_.setWidthF(1.0 / scaling_factor);
    }
    painter->setPen(this->pen_);

    // Draw shape
    painter->fillPath(this->shape(), this->brush_);
    painter->drawEllipse(QRectF(-width, -height, width * 2, height * 2));

    // Draw clearance boundry
    this->clearance_pen_.setWidthF(3.0 / scaling_factor);
    qreal clearance_height =
            height + (this->model_->getClearance() * GRID_SIZE);
    qreal clearance_width =
            width + (this->model_->getClearance() * GRID_SIZE);
    painter->setPen(this->clearance_pen_);
    painter->drawEllipse(QRectF(-clearance_width, -clearance_height,
                                    clearance_width * 2, clearance_height * 2));

    // Label with port
    if (this->model_->port_ != 0) {
        QPointF text_pos(this->mapFromScene(pos));
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
    qreal height = this->model_->getHeight();
    qreal width = this->model_->getWidth();
    path.addEllipse(QRectF(-width, -height, width * 2, height * 2));
    return path;
}

void EllipseGraphicsItem::expandScene() {
    if (this->scene()) {
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

void EllipseGraphicsItem::setRed(bool isOverlap) {
    if (isOverlap) {
        QColor fill = Qt::red;
        fill.setAlpha(200);
        this->brush_ = QBrush(fill);
    } else {
        QColor fill = Qt::gray;
        fill.setAlpha(200);
        this->brush_ = QBrush(fill);
    }
}

void EllipseGraphicsItem::flipDirection() {
    this->model_->flipDirection();
    this->expandScene();
}

QVariant EllipseGraphicsItem::itemChange(GraphicsItemChange change,
                                         const QVariant &value) {
    if (change == ItemPositionChange && this->scene()) {
        // value is the new position
        QPointF newPos = value.toPointF();

        // update model
        this->model_->setPos(newPos);

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

}  // namespace optgui
