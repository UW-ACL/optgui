// TITLE:   Optimization_Interface/include/models/path_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for drone path

#ifndef PATH_MODEL_ITEM_H_
#define PATH_MODEL_ITEM_H_

#include <QPointF>
#include <QVector>
#include <QMutex>

#include "include/models/data_model.h"

namespace interface {

class PathModelItem : public DataModel {
 public:
    PathModelItem() : mutex_()
        { this->points_ = new QVector<QPointF *>(); port_ = 0; }
    ~PathModelItem() {
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

    QPointF *getPointAt(quint64 index) {
        this->mutex_.lock();
        QPointF *temp = this->points_->at(index);
        this->mutex_.unlock();
        return temp;
    }

    void setPoints(QVector<QPointF *> *points) {
        this->mutex_.lock();
        for (QPointF *point : *this->points_) {
            delete point;
        }
        delete this->points_;
        this->points_ = points;
        this->mutex_.unlock();
    }

    void addPoint(QPointF *point) {
        this->mutex_.lock();
        this->points_->append(point);
        this->mutex_.unlock();
    }

    void removePoint(QPointF *point) {
        this->mutex_.lock();
        this->points_->removeOne(point);
        this->mutex_.unlock();
    }

 private:
    QMutex mutex_;
    QVector<QPointF *> *points_;
};

}  // namespace interface

#endif  // PATH_MODEL_ITEM_H_
