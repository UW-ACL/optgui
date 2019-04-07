// TITLE:   Optimization_Interface/plane_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for plane constraint

#ifndef PLANE_MODEL_ITEM_H_
#define PLANE_MODEL_ITEM_H_

#include <QPointF>

namespace interface {

class PlaneModelItem {
 public:
    explicit PlaneModelItem(QPointF *p1, QPointF *p2) :
        direction_(true) { p1_ = p1; p2_ = p2; port_ = 0;}
    ~PlaneModelItem() { delete p1_; delete p2_; }
    QPointF *p1_;
    QPointF *p2_;
    bool direction_;
    quint16 port_;
};

}  // namespace interface

#endif  // PLANE_MODEL_ITEM_H_
