// TITLE:   Optimization_Interface/canvas.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "canvas.h"

#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <cmath>
#include <limits>

namespace interface {

Canvas::Canvas(QObject *parent)
    : QGraphicsScene(parent) {
    this->initialize();
    this->front_depth_ = 0;
}

void Canvas::initialize() {
    // Set background pen
    QColor background_color = Qt::gray;
    background_color.setAlpha(150);
    this->background_pen_ = QPen(background_color);

    // Connect slots
    connect(this, SIGNAL(selectionChanged()), this,
            SLOT(bringSelectedToFront()));
}

void Canvas::bringSelectedToFront() {
    if (!this->selectedItems().isEmpty()) {
        this->selectedItems().first()->setZValue(this->front_depth_);
        this->front_depth_ = std::nextafter(this->front_depth_,
                                            std::numeric_limits<qreal>::max());
    }
}

void Canvas::bringToFront(QGraphicsItem *item) {
    item->setZValue(this->front_depth_);
    this->front_depth_ = std::nextafter(this->front_depth_,
                                        std::numeric_limits<qreal>::max());
}

void Canvas::drawBackground(QPainter *painter, const QRectF &rect) {
    // Get scaling factor
    qreal grids = 1;
    if (!this->views().isEmpty()) {
        grids = this->views().first()->matrix().m11();
    }

    // Add grids proportional to scaling factor
    qreal segment_size = GRID_SIZE;
    qreal pen_width = 2;

    // Set boundries of grid
    qreal top_bound = rect.top() - segment_size;
    qreal bot_bound = rect.bottom() + segment_size;
    qreal left_bound = rect.left() - segment_size;
    qreal right_bound = rect.right() + segment_size;

    // Round to nearest segment_size
    qint32 top = (qint32)(top_bound / segment_size) * segment_size;
    qint32 bot = (qint32)(bot_bound / segment_size) * segment_size;
    qint32 left = (qint32)(left_bound / segment_size) * segment_size;
    qint32 right = (qint32)(right_bound / segment_size) * segment_size;

    for (qint32 i = 0; i < grids; i++) {
        // Set pen
        this->background_pen_.setWidth(pen_width);
        painter->setPen(this->background_pen_);

        // Draw vertical grid lines
        for (qint32 j = 0; j <= right_bound; j += segment_size) {
            painter->drawLine(j , top, j , bot);
        }
        for (qint32 j = 0; j >= left_bound; j -= segment_size) {
            painter->drawLine(j , top, j , bot);
        }

        // Draw horizontal grid lines
        for (qint32 j = 0; j <= bot_bound; j += segment_size) {
            painter->drawLine(left, j , right, j);
        }
        for (qint32 j = 0; j >= top_bound; j -= segment_size) {
            painter->drawLine(left, j , right, j);
        }

        // Update segment_size
        segment_size /= 2;

        // Update pen width
        pen_width /= 2;
    }

    // Draw origin
    painter->drawText(1, -2, "0");

    // Debug info
    // painter->setPen(Qt::red);
    // painter->drawRect(this->sceneRect());
}

}  // namespace interface
