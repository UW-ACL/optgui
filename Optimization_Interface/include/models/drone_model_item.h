// TITLE:   Optimization_Interface/include/models/drone_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for drone

#ifndef DRONE_MODEL_ITEM_H_
#define DRONE_MODEL_ITEM_H_

#include <QPointF>
#include <QVector3D>
#include <QString>
#include <QMutex>

#include "include/models/data_model.h"

#include "include/globals.h"

namespace optgui {

class DroneModelItem : public DataModel {
 public:
    DroneModelItem() : mutex_() {
        this->pos_ = QVector3D(0, 0, 0);
        this->vel_ = QVector3D(0, 0, 0);
        // counteract gravity
        this->accel_ = QVector3D(0, 0, 9.81 * GRID_SIZE);
        port_ = 0;
        destination_port_ = 6000;
        ip_addr_ = "0.0.0.0";
    }

    ~DroneModelItem() {
        QMutexLocker locker(&this->mutex_);
    }

    QVector3D getPos() {
        QMutexLocker locker(&this->mutex_);
        return this->pos_;
    }

    void setPos(QVector3D pos) {
        QMutexLocker locker(&this->mutex_);
        this->pos_ = pos;
    }

    QVector3D getVel() {
        QMutexLocker locker(&this->mutex_);
        return this->vel_;
    }

    void setVel(QVector3D vel) {
        QMutexLocker locker(&this->mutex_);
        this->vel_ = vel;
    }

    QVector3D getAccel() {
        QMutexLocker locker(&this->mutex_);
        return this->accel_;
    }

    void setAccel(QVector3D accel) {
        QMutexLocker locker(&this->mutex_);
        this->accel_ = accel;
    }

    QString ip_addr_;
    quint16 destination_port_;

 private:
    QMutex mutex_;
    QVector3D pos_;
    QVector3D vel_;
    QVector3D accel_;
};

}  // namespace optgui

#endif  // DRONE_MODEL_ITEM_H_
