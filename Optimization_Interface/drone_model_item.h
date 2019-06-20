// TITLE:   Optimization_Interface/drone_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for drone

#ifndef DRONE_MODEL_ITEM_H_
#define DRONE_MODEL_ITEM_H_

#include <QPointF>

#include "data_model.h"

namespace interface {

class DroneModelItem : public DataModel {
 public:
    explicit DroneModelItem()
            { this->point_ = new QPointF(0, 0); port_ = 8000; }
    ~DroneModelItem() { delete this->point_; }
    QPointF *point_;
    QPointF *pos_;
    QPointF *vel_;
};

}  // namespace interface

#endif  // DRONE_MODEL_ITEM_H_
