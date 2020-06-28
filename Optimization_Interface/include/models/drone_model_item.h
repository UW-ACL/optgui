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
    DroneModelItem() : DataModel(), mutex_() {
        // initialize to 0
        this->pos_ = QPointF(0, 0);
        this->vel_ = QPointF(0, 0);
        this->accel_ = QPointF(0, 0);

        // default listening port on mikipilot drones
        destination_port_ = 6000;
        ip_addr_ = "0.0.0.0";
    }

    ~DroneModelItem() {
        // acquire lock to destroy it
        QMutexLocker locker(&this->mutex_);
    }

    QPointF getPos() {
        QMutexLocker locker(&this->mutex_);
        // get copy of pos
        return this->pos_;
    }

    void setPos(QPointF pos) {
        QMutexLocker locker(&this->mutex_);
        this->pos_.setX(pos.x());
        this->pos_.setY(pos.y());
    }

    QPointF getVel() {
        QMutexLocker locker(&this->mutex_);
        // get copy of velocity
        return this->vel_;
    }

    void setVel(QPointF vel) {
        QMutexLocker locker(&this->mutex_);
        this->vel_.setX(vel.x());
        this->vel_.setY(vel.y());
    }

    QPointF getAccel() {
        QMutexLocker locker(&this->mutex_);
        // get copy of acceleration
        return this->accel_;
    }

    void setAccel(QPointF accel) {
        QMutexLocker locker(&this->mutex_);
        this->accel_.setX(accel.x());
        this->accel_.setY(accel.y());
    }

    // IP addr of drone
    QString ip_addr_;
    // listening port on drone
    quint16 destination_port_;

 private:
    // mutex lock for getters/setters
    QMutex mutex_;

    // coords in xyz, pos in pixels
    QPointF pos_;
    QPointF vel_;
    QPointF accel_;
};

}  // namespace optgui

#endif  // DRONE_MODEL_ITEM_H_
