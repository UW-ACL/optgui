// TITLE:   Optimization_Interface/include/models/cylinder_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for cylinder constraint

#ifndef CYLINDER_MODEL_ITEM_H_
#define CYLINDER_MODEL_ITEM_H_

#include <QPointF>
#include <QMutex>
#include <QRegion>
#include <QTransform>

#include "include/models/data_model.h"
#include "include/globals.h"
#include <iostream>

namespace optgui {

qreal const DEFAULT_WIDTH = 20;

class CylinderModelItem : public DataModel {
 public:
    explicit CylinderModelItem(QPointF pos, qreal clearance,
                qreal height = DEFAULT_WIDTH,
                qreal width = DEFAULT_WIDTH,
                qreal trigger_width = DEFAULT_WIDTH,
                qreal rot = 0) :
        DataModel(), mutex_(), height_(height), width_(width), trigger_width_(trigger_width), rot_(rot),
        direction_(false), is_overlap_(false), clearance_(clearance) {
        // set pos from param
        this->pos_ = pos;
        // generate region for overlap detection,
        // only generate new region when coords change
        this->region_ = generateRegion();
    }

    ~CylinderModelItem() {
        // acquire lock to destroy it
        QMutexLocker locker(&this->mutex_);
    }

    qreal getWidth() {
        QMutexLocker locker(&this->mutex_);
        // get width in pixels
        return this->width_;
    }

    qreal getTriggerWidth() {
        QMutexLocker locker(&this->mutex_);
        // get width in pixels
        return this->trigger_width_;
    }

    void setWidth(qreal width) {
        QMutexLocker locker(&this->mutex_);
        this->width_ = width;
        // update region
        this->region_ = this->generateRegion();
    }

    void setTriggerWidth(qreal trigger_width) {
        QMutexLocker locker(&this->mutex_);
        this->trigger_width_ = std::max(0.0,trigger_width);
    }
    qreal getHeight() {
        QMutexLocker locker(&this->mutex_);
        // get height in pixels
        return this->height_;
    }

    void setHeight(qreal height) {
        QMutexLocker locker(&this->mutex_);
        this->height_ = height;
        // update region
        this->region_ = this->generateRegion();
    }

    qreal getRot() {
        QMutexLocker locker(&this->mutex_);
        // get rotation in degrees clockwise
        return this->rot_;
    }

    void setRot(qreal rot) {
        QMutexLocker locker(&this->mutex_);
        this->rot_ = rot;
        // update region
        this->region_ = this->generateRegion();
    }

    QPointF getPos() {
        QMutexLocker locker(&this->mutex_);
        // get copy of pos in xyz pixels
        return this->pos_;
    }

    void setPos(QPointF pos) {
        QMutexLocker locker(&this->mutex_);
        this->pos_.setX(pos.x());
        this->pos_.setY(pos.y());
        // update region
        this->region_ = this->generateRegion();
    }

    bool getDirection() {
        QMutexLocker locker(&this->mutex_);
        // get direction of constraint inequality
        // (not currently supported by socp)
        return this->direction_;
    }

    void flipDirection() {
        QMutexLocker locker(&this->mutex_);
        // flip direction of constraint inequality
        this->direction_ = !this->direction_;
    }

    qreal getClearance() {
        QMutexLocker locker(&this->mutex_);
        // get clearance around obs in meters
        return this->clearance_;
    }

    void setClearance(qreal clearance) {
        QMutexLocker locker(&this->mutex_);
        this->clearance_ = clearance;
        // update region
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
    // mutex lock for getters and setters
    QMutex mutex_;
    // dimensions in pixels
    qreal height_;
    qreal width_;
    qreal trigger_width_;
    // rotation clockwise in degrees
    qreal rot_;
    // position in xyz pixels
    QPointF pos_;
    // direction of constraint
    bool direction_;
    // flag for overlapping cylinder
    bool is_overlap_;
    // clearance in meters
    qreal clearance_;
    // save region for detecting overlap to minimize
    // times needed to generate
    QRegion region_;

    QRegion generateRegion() {
        // create a QRegion to use for overlap detection
        qreal width = this->width_ + (this->clearance_ * GRID_SIZE);
        qreal height = this->height_ + (this->clearance_ * GRID_SIZE);
        QRegion region = QRegion(QRect(-width, -height,
                                       width * 2, height * 2),
                                 QRegion::Rectangle);
        QTransform rotation;
        rotation.rotate(this->rot_);
        region = rotation.map(region);
        region.translate(this->pos_.x(), this->pos_.y());

        return region;
    }
};

}  // namespace optgui

#endif  // CYLINDER_MODEL_ITEM_H_
