// TITLE:   Optimization_Interface/ellipse_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for ellipse constraint

#ifndef ELLIPSE_MODEL_ITEM_H_
#define ELLIPSE_MODEL_ITEM_H_

#include <QPointF>

#include "data_model.h"

namespace interface {

const double DEFAULT_RAD = 50;

class EllipseModelItem : public DataModel {
 public:
    explicit EllipseModelItem(QPointF *pos) : radius_(DEFAULT_RAD),
        direction_(true) { pos_ = pos; port_ = 0; clearance_ = 20;}
    ~EllipseModelItem() { delete pos_; }
    double radius_;
    QPointF *pos_;
    double clearance_;
    bool direction_;
};

}  // namespace interface

#endif  // ELLIPSE_MODEL_ITEM_H_
