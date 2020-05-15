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
        this->mutex_.lock();
        this->mutex_.unlock();
    }

    qreal getWidth() {
        this->mutex_.lock();
        qreal temp = this->width_;
        this->mutex_.unlock();
        return temp;
    }

    void setWidth(qreal width) {
        this->mutex_.lock();
        this->width_ = width;
        this->region_ = this->generateRegion();
        this->mutex_.unlock();
    }

    qreal getHeight() {
        this->mutex_.lock();
        qreal temp = this->height_;
        this->mutex_.unlock();
        return temp;
    }

    void setHeight(qreal height) {
        this->mutex_.lock();
        this->height_ = height;
        this->region_ = this->generateRegion();
        this->mutex_.unlock();
    }

    qreal getRot() {
        this->mutex_.lock();
        qreal temp = this->rot_;
        this->mutex_.unlock();
        return temp;
    }

    void setRot(qreal rot) {
        this->mutex_.lock();
        this->rot_ = rot;
        this->region_ = this->generateRegion();
        this->mutex_.unlock();
    }

    QPointF getPos() {
        this->mutex_.lock();
        QPointF temp = this->pos_;
        this->mutex_.unlock();
        return temp;
    }

    void setPos(QPointF pos) {
        this->mutex_.lock();
        this->pos_.setX(pos.x());
        this->pos_.setY(pos.y());
        this->region_ = this->generateRegion();
        this->mutex_.unlock();
    }

    bool getDirection() {
        this->mutex_.lock();
        qreal temp = this->direction_;
        this->mutex_.unlock();
        return temp;
    }

    void flipDirection() {
        this->mutex_.lock();
        this->direction_ = !this->direction_;
        this->mutex_.unlock();
    }

    qreal getClearance() {
        this->mutex_.lock();
        qreal temp = this->clearance_;
        this->mutex_.unlock();
        return temp;
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
