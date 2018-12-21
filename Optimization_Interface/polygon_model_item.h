// TITLE:   Optimization_Interface/polygon_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef POLYGON_MODEL_ITEM_H
#define POLYGON_MODEL_ITEM_H

#include <QVector>
#include <QPointF>

namespace interface
{

class PolygonModelItem
{
public:
    PolygonModelItem(QVector<QPointF *> *points) : convex_(true) { points_ = points; }
    ~PolygonModelItem() {
        for (QPointF *point : *this->points_) {
            delete point;
        }
        delete this->points_;
    }
    QVector<QPointF *> *points_;
    bool convex_;
};

}

#endif // POLYGON_MODEL_ITEM_H
