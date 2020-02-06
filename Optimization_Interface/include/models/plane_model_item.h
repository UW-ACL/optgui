// TITLE:   Optimization_Interface/include/models/plane_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for plane constraint

#ifndef PLANE_MODEL_ITEM_H_
#define PLANE_MODEL_ITEM_H_

#include <QPointF>

#include "include/models/data_model.h"

namespace interface {

class PlaneModelItem : public DataModel {
 public:
    explicit PlaneModelItem(QPointF *p1, QPointF *p2) :
        mutex_(), direction_(false) { p1_ = p1; p2_ = p2; port_ = 0;}
    ~PlaneModelItem() {
        this->mutex_.lock();
        delete p1_;
        delete p2_;
        this->mutex_.unlock();
    }

    QPointF getP1() {
        this->mutex_.lock();
        QPointF temp = *this->p1_;
        this->mutex_.unlock();
        return temp;
    }

    void setP1(QPointF pos) {
        this->mutex_.lock();
        this->p1_->setX(pos.x());
        this->p1_->setY(pos.y());
        this->mutex_.unlock();
    }

    QPointF getP2() {
        this->mutex_.lock();
        QPointF temp = *this->p2_;
        this->mutex_.unlock();
        return temp;
    }

    void setP2(QPointF pos) {
        this->mutex_.lock();
        this->p2_->setX(pos.x());
        this->p2_->setY(pos.y());
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
 private:
    QMutex mutex_;
    QPointF *p1_;
    QPointF *p2_;
    bool direction_;
};

}  // namespace interface

#endif  // PLANE_MODEL_ITEM_H_
