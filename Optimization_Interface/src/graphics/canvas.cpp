// TITLE:   Optimization_Interface/src/graphics/canvas.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/canvas.h"

#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsItem>

#include <cmath>
#include <limits>

#include "include/globals.h"

namespace optgui {

Canvas::Canvas(QObject *parent, QString background_file)
    : QGraphicsScene(parent) {

    // set background brush
    this->setBackgroundBrush(BLACK);
    // Set background pen
    QColor background_color = Qt::gray;
    background_color.setAlpha(150);
    this->background_pen_ = QPen(background_color);

    // Set foreground pen
    QColor foreground_color = Qt::gray;
    // foreground_color.setAlpha(250);
    this->foreground_pen_ = QPen(foreground_color);

    // Set text font
    this->font_ = QFont("SansSerif");

    // Set flags
    this->setItemIndexMethod(QGraphicsScene::ItemIndexMethod::NoIndex);

    // Connect slots
    connect(this, SIGNAL(selectionChanged()), this,
            SLOT(bringSelectedToFront()));

    this->front_depth_ = 0;
    this->setBackgroundImage(background_file);
}

Canvas::~Canvas() {
}

qreal Canvas::getScalingFactor() {
    qreal scale = 1;
    if (!this->views().isEmpty()) {
        qreal matrix = qMin(this->views().first()->matrix().m11(), 1.0);
        quint64 factor = quint64(GRID_SIZE / matrix) / GRID_SIZE;
        scale = 1.0 / factor;
    }
    return scale;
}

void Canvas::setBackgroundImage(QString filename) {
    QStringList list = filename.split('_');
    if (list.length() != 6) {
        // qDebug() << "Image filename not formatted correctly";
    }

    // set background image location
    if (list[1] == "outdoor") {
        this->background_bottomleft_x_ = 0;  // mikipilot pos_ref_ned uses bottomleft as origin for outdoor
        this->background_bottomleft_y_ = 0;  // mikipilot pos_ref_ned uses bottomleft as origin for outdoor
        this->background_topright_x_ = list[4].toDouble();
        this->background_topright_y_ = list[5].toDouble();
    } else if (list[1] == "indoor") {
        this->background_bottomleft_x_ = list[2].toDouble();
        this->background_bottomleft_y_ = list[3].toDouble();
        this->background_topright_x_ = list[4].toDouble();
        this->background_topright_y_ = list[5].toDouble();
    }

    this->background_image_ = QImage(":/assets/" + filename + ".png");
}

void Canvas::bringSelectedToFront() {
    if (!this->selectedItems().isEmpty()) {
        // grab first selected item
        QGraphicsItem *selected = this->selectedItems().first();
        if (selected->type() == ELLIPSE_GRAPHIC ||
                selected->type() == CYLINDER_GRAPHIC ||
                selected->type() == POLYGON_GRAPHIC ||
                selected->type() == PLANE_GRAPHIC) {
            // move to front render level if it is a constraint graphic
            this->selectedItems().first()->setZValue(this->front_depth_);
            // update the highest render level
            this->front_depth_ = std::nextafter(this->front_depth_,
                    std::numeric_limits<qreal>::max());
        }
    }
}

void Canvas::updateGraphicsItems(PathGraphicsItem *traj,
                                 DroneGraphicsItem *drone) {
    // verify graphics exist
    if (this->path_graphics_.contains(traj) &&
        this->drone_graphics_.contains(drone)) {
        // schedule re-draw
        traj->update(traj->boundingRect());
        drone->update(drone->boundingRect());
    }
}

void Canvas::bringToFront(QGraphicsItem *item) {
    if (item->type() == ELLIPSE_GRAPHIC ||
            item->type() == CYLINDER_GRAPHIC ||
            item->type() == POLYGON_GRAPHIC ||
            item->type() == PLANE_GRAPHIC) {
        // move given item to front if it is a constraint graphic
        item->setZValue(this->front_depth_);
        // update the highest render level
        this->front_depth_ = std::nextafter(this->front_depth_,
                                            std::numeric_limits<qreal>::max());
    }
}

void Canvas::drawForeground(QPainter *painter, const QRectF &rect) {
    // get meters scale
    qreal scale = this->getScalingFactor();
    qint32 segment_size = GRID_SIZE;
    segment_size /= scale;

    // scale pens and meter scale
    qreal pen_width = 2;
    qreal font_size = 20 / scale;
    qint32 offset = 40 / scale;
    qint32 text_offset = 50 / scale;
    qint32 notch_offset = 45 / scale;

    this->foreground_pen_.setWidthF(pen_width);
    this->font_.setPixelSize(font_size);
    painter->setPen(this->foreground_pen_);
    painter->setFont(this->font_);

    // Draw scale
    painter->drawLine(rect.left() + offset, rect.bottom() - offset,
                      rect.left() + (offset + segment_size),
                      rect.bottom() - offset);
    // Draw notches on scale
    for (qint32 i = 0; i <= segment_size; i += segment_size) {
        painter->drawLine(rect.left() + offset + i, rect.bottom() - offset,
                          rect.left() + offset + i,
                          rect.bottom() - notch_offset);
    }

    // Draw label
    painter->drawText(rect.left() + offset, rect.bottom() - text_offset,
                      QString::number(qreal(segment_size) / GRID_SIZE) + "m");
}

qint64 Canvas::roundUpPast(qint64 n, qint64 m) {
    // return next integer greater than n some multiple of m
    return ((n + m - 1) / m) * m;
}

qint64 Canvas::roundDownPast(qint64 n, qint64 m) {
    // return next integer less than n some multiple of m
    return ((n - m + 1) / m) * m;
}

void Canvas::drawBackground(QPainter *painter, const QRectF &rect) {
    // Fill background
    QGraphicsScene::drawBackground(painter, rect);

    // Add grids proportional to scaling factor
    qreal scale = this->getScalingFactor();
    qint64 segment_size = GRID_SIZE;
    segment_size /= scale;

    qreal pen_width = 2.0;
    qreal font_size = 20 / scale;

    // Set boundries of grid
    qint64 top_bound = roundDownPast(qRound64(rect.top()), segment_size);
    qint64 bot_bound = roundUpPast(qRound64(rect.bottom()), segment_size);
    qint64 left_bound = roundDownPast(qRound64(rect.left()), segment_size);
    qint64 right_bound = roundUpPast(qRound64(rect.right()), segment_size);

    // Set pen
    this->background_pen_.setWidthF(pen_width);
    this->font_.setPixelSize(font_size);
    painter->setPen(this->background_pen_);
    painter->setFont(this->font_);

    // calculate position of background image
    double width  = this->background_topright_y_
            - this->background_bottomleft_y_;
    double height = this->background_topright_x_
            - this->background_bottomleft_x_;

    QRectF bbox(this->background_bottomleft_y_*GRID_SIZE,
                -this->background_topright_x_*GRID_SIZE,
                width*GRID_SIZE,
                height*GRID_SIZE);

    // draw background image
    painter->drawImage(bbox, this->background_image_);

    // Draw vertical grid lines
    for (qint32 i = 0; i <= right_bound; i += segment_size) {
        painter->drawLine(i, top_bound, i, bot_bound);
    }
    for (qint32 i = 0; i >= left_bound; i -= segment_size) {
        painter->drawLine(i, top_bound, i , bot_bound);
    }

    // Draw horizontal grid lines
    for (qint32 i = 0; i <= bot_bound; i += segment_size) {
        painter->drawLine(left_bound, i , right_bound, i);
    }
    for (qint32 i = 0; i >= top_bound; i -= segment_size) {
        painter->drawLine(left_bound, i, right_bound, i);
    }

    // Draw origin coordinate
    painter->drawText(1, -2, "0");
}

}  // namespace optgui
