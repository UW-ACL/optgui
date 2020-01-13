// TITLE:   Optimization_Interface/include/models/drone_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for drone

#ifndef DRONE_MODEL_ITEM_H_
#define DRONE_MODEL_ITEM_H_

#include <QPointF>

#include "include/models/data_model.h"

namespace interface {

class DroneModelItem : public DataModel {
 public:
    DroneModelItem()
            { this->pos_ = new QPointF(0, 0); port_ = 0; }
    ~DroneModelItem() { delete this->pos_; }
    QPointF *pos_;
};

}  // namespace interface

#endif  // DRONE_MODEL_ITEM_H_
