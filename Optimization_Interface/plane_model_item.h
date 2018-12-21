// TITLE:   Optimization_Interface/plane_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef PLANE_MODEL_ITEM_H
#define PLANE_MODEL_ITEM_H

#include <QPointF>

namespace interface
{

class PlaneModelItem
{
public:
    PlaneModelItem(QPointF *p1, QPointF *p2) : convex_(true) { p1_ = p1; p2_ = p2; }
    ~PlaneModelItem() { delete p1_; delete p2_; }
    QPointF *p1_;
    QPointF *p2_;
    bool convex_;
};

}

#endif // PLANE_MODEL_ITEM_H
