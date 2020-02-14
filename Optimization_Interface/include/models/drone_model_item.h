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
        port_ = 0;
        destination_port_ = 0;
        ip_addr_ = "0.0.0.0";
    }
    ~DroneModelItem() {
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

    QString ip_addr_;
    quint16 destination_port_;

 private:
    QMutex mutex_;
    QPointF pos_;
};

}  // namespace optgui

#endif  // DRONE_MODEL_ITEM_H_
