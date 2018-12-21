// TITLE:   Optimization_Interface/ellipse_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef ELLIPSE_MODEL_ITEM_H
#define ELLIPSE_MODEL_ITEM_H

#include <QPointF>

namespace interface
{

const double DEFAULT_RAD = 50;

class EllipseModelItem
{
public:
    EllipseModelItem(QPointF *pos) : radius_(DEFAULT_RAD), convex_(true) { pos_ = pos; }
    ~EllipseModelItem() { delete pos_; }
    double radius_;
    QPointF *pos_;
    bool convex_;
};

}

#endif // ELLIPSE_MODEL_ITEM_H
