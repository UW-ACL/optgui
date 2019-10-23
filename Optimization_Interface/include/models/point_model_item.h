// TITLE:   Optimization_Interface/include/models/point_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for point

#ifndef POINT_MODEL_ITEM_H_
#define POINT_MODEL_ITEM_H_

#include <QPointF>

#include "data_model.h"

namespace interface {

const double DEFAULT_RAD2 = 5;

class PointModelItem : public DataModel {
 public:
    PointModelItem() { pos_ = new QPointF(0, 0); port_ = 0; }
    explicit PointModelItem(QPointF *pos) { pos_ = pos; port_ = 0;}
    ~PointModelItem() { delete pos_; }
    QPointF *pos_;
    bool direction_;
};

}  // namespace interface

#endif  // POINT_MODEL_ITEM_H_
