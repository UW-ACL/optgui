// TITLE:   Optimization_Interface/include/models/polygon_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for polygon constraint

#ifndef POLYGON_MODEL_ITEM_H
#define POLYGON_MODEL_ITEM_H

#include <QVector>
#include <QPointF>

#include "include/models/data_model.h"

namespace interface {

class PolygonModelItem : public DataModel {
 public:
    explicit PolygonModelItem(QVector<QPointF *> *points) :
        direction_(false) { points_ = points; port_ = 0;}
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
