// TITLE:   Optimization_Interface/include/models/ellipse_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for ellipse constraint

#ifndef ELLIPSE_MODEL_ITEM_H_
#define ELLIPSE_MODEL_ITEM_H_

#include <QPointF>
#include <QMutex>

#include "include/models/data_model.h"

namespace optgui {

qreal const DEFAULT_RAD = 100;

class EllipseModelItem : public DataModel {
 public:
    explicit EllipseModelItem(QPointF pos, qreal *clearance,
                              qreal radius = DEFAULT_RAD) :
        mutex_(), radius_(radius), direction_(false), clearance_(clearance) {
        pos_ = pos;
        port_ = 0;
    }

    ~EllipseModelItem() {
        this->mutex_.lock();
        this->mutex_.unlock();
    }

    qreal getRadius() {
        this->mutex_.lock();
        qreal temp = this->radius_;
        this->mutex_.unlock();
        return temp;
    }

    void setRadius(qreal radius) {
        this->mutex_.lock();
        this->radius_ = radius;
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
        qreal temp = *this->clearance_;
        this->mutex_.unlock();
        return temp;
    }

 private:
    QMutex mutex_;
    qreal radius_;
    QPointF pos_;
    bool direction_;
    qreal *clearance_;
};

}  // namespace optgui

#endif  // ELLIPSE_MODEL_ITEM_H_
