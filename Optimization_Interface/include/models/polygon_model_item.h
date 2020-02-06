// TITLE:   Optimization_Interface/include/models/polygon_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for polygon constraint

#ifndef POLYGON_MODEL_ITEM_H
#define POLYGON_MODEL_ITEM_H

#include <QVector>
#include <QPointF>
#include <QMutex>

#include "include/models/data_model.h"

namespace interface {

class PolygonModelItem : public DataModel {
 public:
    explicit PolygonModelItem(QVector<QPointF *> *points) :
        mutex_(), direction_(false) { points_ = points; port_ = 0;}
    ~PolygonModelItem() {
        this->mutex_.lock();
        for (QPointF *point : *this->points_) {
            delete point;
        }
        delete this->points_;
        this->mutex_.unlock();
    }

    quint64 getSize() {
        this->mutex_.lock();
        quint64 size = this->points_->size();
        this->mutex_.unlock();
        return size;
    }

    void setPointAt(QPointF point, quint64 index) {
        this->mutex_.lock();
        QPointF *temp = this->points_->at(index);
        temp->setX(point.x());
        temp->setY(point.y());
        this->mutex_.unlock();
    }

    QPointF getPointAt(quint64 index) {
        this->mutex_.lock();
        QPointF temp = *this->points_->at(index);
        this->mutex_.unlock();
        return temp;
    }

    bool getDirection() {
        this->mutex_.lock();
        qreal temp = this->direction_;
        this->mutex_.unlock();
        return temp;
    }

    void flipDirection() {
        this->mutex_.lock();
        this->direction_ = !this->direction_;
        this->mutex_.unlock();
    }

 private:
    QMutex mutex_;
    QVector<QPointF *> *points_;
    bool direction_;
};

}  // namespace interface

#endif  // POLYGON_MODEL_ITEM_H_
