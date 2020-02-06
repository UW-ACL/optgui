// TITLE:   Optimization_Interface/include/models/ellipse_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for ellipse constraint

#ifndef ELLIPSE_MODEL_ITEM_H_
#define ELLIPSE_MODEL_ITEM_H_

#include <QPointF>

#include "include/models/data_model.h"

namespace interface {

qreal const DEFAULT_RAD = 100;

class EllipseModelItem : public DataModel {
 public:
    explicit EllipseModelItem(QPointF *pos, qreal radius = DEFAULT_RAD) :
        radius_(radius), direction_(false) { pos_ = pos; port_ = 0; }
    ~EllipseModelItem() { delete pos_; }
    double radius_;
    QPointF *pos_;
    bool direction_;
};

}  // namespace interface

#endif  // ELLIPSE_MODEL_ITEM_H_
