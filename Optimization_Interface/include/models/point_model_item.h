// TITLE:   Optimization_Interface/include/models/point_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for point

#ifndef POINT_MODEL_ITEM_H_
#define POINT_MODEL_ITEM_H_

#include <QPointF>
#include <QMutex>

#include "include/models/data_model.h"

namespace optgui {

const double DEFAULT_RAD2 = 5;

class PointModelItem : public DataModel {
 public:
    PointModelItem() : mutex_() { pos_ = QPointF(0, 0); port_ = 0; }
    ~PointModelItem() {
        this->mutex_.lock();
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

 private:
    QMutex mutex_;
    QPointF pos_;
};

}  // namespace optgui

#endif  // POINT_MODEL_ITEM_H_
