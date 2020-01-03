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

namespace interface {

Canvas::Canvas(QObject *parent, QString background_file)
    : QGraphicsScene(parent) {
    this->initialize();
    this->front_depth_ = 0;
    this->setBackgroundImage(background_file);
}

void Canvas::initialize() {
    this->setBackgroundBrush(Qt::black);
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
}

void Canvas::setBackgroundImage(QString filename) {
//     QString filename = ";
//    QString filename = ;
    QStringList list = filename.split('_');
    if (list.length() != 6) {
        // qDebug() << "Image filename not formatted correctly";
    }

    if (list[1] == "outdoor") {
//        qDebug() << "Outdoor mode: lat" << list[2].toFloat() << "lon:"
//                 << list[3].toFloat() << "width:" << list[4].toFloat()
//                 << "height:" << list[5].toFloat();
        this->background_bottomleft_x_ = 0;
        this->background_bottomleft_y_ = 0;  // list[5].toFloat();
        this->background_topright_x_ = list[4].toFloat();
        this->background_topright_y_ = list[5].toFloat();
        this->indoor_ = false;

    } else if (list[1] == "indoor") {
        this->background_bottomleft_x_ = list[2].toFloat();
        this->background_bottomleft_y_ = list[3].toFloat();
        this->background_topright_x_ = list[4].toFloat();
        this->background_topright_y_ = list[5].toFloat();
//        qDebug() << "Indoor mode:" << "bottom left"
//                 << this->background_bottomleft_x_
//                 << this->background_bottomleft_y_
//                 << "top right:" << this->background_topright_x_
//                 << this->background_topright_y_;
        this->indoor_ = true;
    }

    this->background_image_ =
            new QImage("../../../../skyenet/maps/" + filename + ".png");
}

QPointF* Canvas::getBottomLeft() {
    return new QPointF(this->background_bottomleft_y_*this->scale_,
                       -this->background_bottomleft_x_*this->scale_);
}

QPointF* Canvas::getTopRight() {
    return new QPointF(this->background_topright_y_*this->scale_,
                       -this->background_topright_x_*this->scale_);
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

void Canvas::expandScene() {
    for (QGraphicsItem *item : this->items()) {
        QRectF newRect = item->sceneBoundingRect();
        QRectF rect = this->sceneRect();
        if (!rect.contains(newRect)) {
            this->setSceneRect(this->sceneRect().united(newRect));
            if (!this->views().isEmpty()) {
                this->views().first()->setSceneRect(
                            this->sceneRect());
            }
            this->update();
        }
    }
}

void Canvas::drawForeground(QPainter *painter, const QRectF &rect) {
    // Get scaling factor
    qreal scale = 1;
    if (!this->views().isEmpty()) {
        scale = this->views().first()->matrix().m11();
    }

    qint32 segment_size = GRID_SIZE;
    if (scale < 0.5) {
        segment_size /= 0.5;
    } else if (scale > 1.5) {
        segment_size /= 2;
    }

    qreal pen_width = 2 / scale;
    qreal font_size = 20 / scale;
    qint32 offset = 40 / scale;
    qint32 text_offset = 50 / scale;
    qint32 notch_offset = 45 / scale;

    this->foreground_pen_.setWidthF(pen_width);
    this->font_.setPointSizeF(font_size);
    painter->setPen(this->foreground_pen_);
    painter->setFont(this->font_);

    // Draw scale
    painter->drawLine(rect.left() + offset, rect.bottom() - offset,
                      rect.left() + (offset + segment_size),
                      rect.bottom() - offset);
    // Draw notches on scale
    for (qint32 i = 0; i <= segment_size; i += GRID_SIZE / 2) {
        painter->drawLine(rect.left() + offset + i, rect.bottom() - offset,
                          rect.left() + offset + i,
                          rect.bottom() - notch_offset);
    }

    // Draw label
    painter->drawText(rect.left() + offset, rect.bottom() - text_offset,
                      QString::number(qreal(segment_size) / 100) + UNIT);
}

qint64 Canvas::roundUpPast(qint64 n, qint64 m) {
    return ((n + m - 1) / m) * m;
}

qint64 Canvas::roundDownPast(qint64 n, qint64 m) {
    return ((n - m + 1) / m) * m;
}

void Canvas::drawBackground(QPainter *painter, const QRectF &rect) {
    // Get scaling factor
    qreal scale = 1;
    if (!this->views().isEmpty()) {
        scale = this->views().first()->matrix().m11();
    }

    // Expand scene to fit exposed area
    this->setSceneRect(this->sceneRect().united(rect));
    if (!this->views().isEmpty()) {
        this->views().first()->setSceneRect(this->sceneRect().united(rect));
    }

    // Add grids proportional to scaling factor
    qint64 segment_size = GRID_SIZE;
    if (scale < 0.5) {
        segment_size /= 0.5;
    } else if (scale > 1.5) {
        segment_size /= 2;
    }

    qreal pen_width = 2 / scale;
    qreal font_size = 20 / scale;

    // Set boundries of grid
    qint64 top_bound = roundDownPast(qRound64(rect.top()), segment_size);
    qint64 bot_bound = roundUpPast(qRound64(rect.bottom()), segment_size);
    qint64 left_bound = roundDownPast(qRound64(rect.left()), segment_size);
    qint64 right_bound = roundUpPast(qRound64(rect.right()), segment_size);

    // Set pen
    this->background_pen_.setWidthF(pen_width);
    this->font_.setPointSizeF(font_size);
    painter->setPen(this->background_pen_);
    painter->setFont(this->font_);

    double width  = this->background_topright_y_
            - this->background_bottomleft_y_;
    double height = this->background_topright_x_
            - this->background_bottomleft_x_;

    QRectF bbox(this->background_bottomleft_y_*this->scale_,
                -this->background_topright_x_*this->scale_,
                width*this->scale_,
                height*this->scale_);

    painter->drawImage(bbox, *this->background_image_);

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

    // Draw origin
    painter->drawText(1, -2, "0");

    // Debug info
//    painter->setPen(Qt::red);
//    painter->drawRect(this->sceneRect());
//    painter->setPen(Qt::blue);
//    if (!this->views().isEmpty()) {
//        painter->drawRect(this->views().first()->sceneRect());
//    }
}

}  // namespace interface
