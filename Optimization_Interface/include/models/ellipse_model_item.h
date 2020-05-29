// TITLE:   Optimization_Interface/include/models/ellipse_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for ellipse constraint

#ifndef ELLIPSE_MODEL_ITEM_H_
#define ELLIPSE_MODEL_ITEM_H_

#include <QPointF>
#include <QMutex>
#include <QRegion>
#include <QTransform>

#include "include/models/data_model.h"
#include "include/globals.h"

namespace optgui {

qreal const DEFAULT_RAD = 100;

class EllipseModelItem : public DataModel {
 public:
    explicit EllipseModelItem(QPointF pos, qreal clearance,
                qreal height = DEFAULT_RAD,
                qreal width = DEFAULT_RAD, qreal rot = 0) :
        mutex_(), height_(height), width_(width), rot_(rot),
        direction_(false), is_overlap_(false), clearance_(clearance) {
        this->pos_ = pos;
        this->port_ = 0;
        this->region_ = generateRegion();
    }

    ~EllipseModelItem() {
        QMutexLocker locker(&this->mutex_);
    }

    qreal getWidth() {
        QMutexLocker locker(&this->mutex_);
        return this->width_;
    }

    void setWidth(qreal width) {
        QMutexLocker locker(&this->mutex_);
        this->width_ = width;
        this->region_ = this->generateRegion();
    }

    qreal getHeight() {
        QMutexLocker locker(&this->mutex_);
        return this->height_;
    }

    void setHeight(qreal height) {
        QMutexLocker locker(&this->mutex_);
        this->height_ = height;
        this->region_ = this->generateRegion();
    }

    qreal getRot() {
        QMutexLocker locker(&this->mutex_);
        return this->rot_;
    }

    void setRot(qreal rot) {
        QMutexLocker locker(&this->mutex_);
        this->rot_ = rot;
        this->region_ = this->generateRegion();
    }

    QPointF getPos() {
        QMutexLocker locker(&this->mutex_);
        return this->pos_;
    }

    void setPos(QPointF pos) {
        QMutexLocker locker(&this->mutex_);
        this->pos_.setX(pos.x());
        this->pos_.setY(pos.y());
        this->region_ = this->generateRegion();
    }

    bool getDirection() {
        QMutexLocker locker(&this->mutex_);
        return this->direction_;
    }

    void flipDirection() {
        QMutexLocker locker(&this->mutex_);
        this->direction_ = !this->direction_;
    }

    qreal getClearance() {
        QMutexLocker locker(&this->mutex_);
        return this->clearance_;
    }

    void setClearance(qreal clearance) {
        QMutexLocker locker(&this->mutex_);
        this->clearance_ = clearance;
        this->region_ = this->generateRegion();
    }

    bool getIsOverlap() {
        QMutexLocker locker(&this->mutex_);
        return this->is_overlap_;
    }

    void setIsOverlap(bool is_overlap) {
        QMutexLocker locker(&this->mutex_);
        this->is_overlap_ = is_overlap;
    }

    QRegion const getRegion() {
        QMutexLocker locker(&this->mutex_);
        return this->region_;
    }

 private:
    QMutex mutex_;
    qreal height_;
    qreal width_;
    qreal rot_;
    QPointF pos_;
    bool direction_;
    bool is_overlap_;
    qreal clearance_;
    QRegion region_;

    QRegion generateRegion() {
        qreal width = this->width_ + (this->clearance_ * GRID_SIZE);
        qreal height = this->height_ + (this->clearance_ * GRID_SIZE);
        QRegion region = QRegion(QRect(-width, -height,
                                       width * 2, height * 2),
                                 QRegion::Ellipse);
        QTransform rotation;
        rotation.rotate(this->rot_);
        region = rotation.map(region);
        region.translate(this->pos_.x(), this->pos_.y());

        return region;
    }
};

}  // namespace optgui

#endif  // ELLIPSE_MODEL_ITEM_H_
