// TITLE:   Optimization_Interface/drone_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for drone

#ifndef DRONE_MODEL_ITEM_H_
#define DRONE_MODEL_ITEM_H_

#include <QPointF>

namespace interface {

class DroneModelItem {
 public:
    explicit DroneModelItem() :
        point_(new QPointF()), port_(0) {}
    ~DroneModelItem() { delete this->point_;}
    QPointF *point_;
    quint16 port_;
};

}  // namespace interface

#endif  // DRONE_MODEL_ITEM_H_
