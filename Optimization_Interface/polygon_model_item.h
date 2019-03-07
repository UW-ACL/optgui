// TITLE:   Optimization_Interface/polygon_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for polygon constraint

#ifndef POLYGON_MODEL_ITEM_H
#define POLYGON_MODEL_ITEM_H

#include <QVector>
#include <QPointF>

namespace interface {

class PolygonModelItem {
 public:
    explicit PolygonModelItem(QVector<QPointF *> *points) :
        direction_(true) { points_ = points; }
    ~PolygonModelItem() {
        for (QPointF *point : *this->points_) {
            delete point;
        }
        delete this->points_;
    }
    QVector<QPointF *> *points_;
    bool direction_;
};

}  // namespace interface

#endif  // POLYGON_MODEL_ITEM_H_
