// TITLE:   Optimization_Interface/path_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for drone path

#ifndef PATH_MODEL_ITEM_H_
#define PATH_MODEL_ITEM_H_

#include <QPointF>

namespace interface {

class PathModelItem {
 public:
    explicit PathModelItem() :
        points_(new QVector<QPointF *>()), port_(0) {}
    ~PathModelItem() {
        for (QPointF *point : *this->points_) {
            delete point;
        }
        delete this->points_;
    }
    QVector<QPointF *> *points_;
    quint16 port_;
};

}  // namespace interface

#endif  // PATH_MODEL_ITEM_H_
