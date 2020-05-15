// TITLE:   Optimization_Interface/include/models/drone_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for drone

#ifndef DRONE_MODEL_ITEM_H_
#define DRONE_MODEL_ITEM_H_

#include <QPointF>
#include <QString>
#include <QMutex>

#include "include/models/data_model.h"

namespace optgui {

class DroneModelItem : public DataModel {
 public:
    DroneModelItem() : mutex_() {
        this->pos_ = QPointF(0, 0);
        this->vel_ = QPointF(0, 0);
        this->accel_ = QPointF(0, 0);
        port_ = 0;
        destination_port_ = 6000;
        ip_addr_ = "0.0.0.0";
    }

    ~DroneModelItem() {
        QMutexLocker locker(&this->mutex_);
    }

    QPointF getPos() {
        QMutexLocker locker(&this->mutex_);
        return this->pos_;
    }

    void setPos(QPointF pos) {
        QMutexLocker locker(&this->mutex_);
        this->pos_.setX(pos.x());
        this->pos_.setY(pos.y());
    }

    QPointF getVel() {
        QMutexLocker locker(&this->mutex_);
        return this->vel_;
    }

    void setVel(QPointF vel) {
        QMutexLocker locker(&this->mutex_);
        this->vel_.setX(vel.x());
        this->vel_.setY(vel.y());
    }

    QPointF getAccel() {
        QMutexLocker locker(&this->mutex_);
        return this->accel_;
    }

    void setAccel(QPointF accel) {
        QMutexLocker locker(&this->mutex_);
        this->accel_.setX(accel.x());
        this->accel_.setY(accel.y());
    }

    QString ip_addr_;
    quint16 destination_port_;

 private:
    QMutex mutex_;
    QPointF pos_;
    QPointF vel_;
    QPointF accel_;
};

}  // namespace optgui

#endif  // DRONE_MODEL_ITEM_H_
