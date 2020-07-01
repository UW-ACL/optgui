// TITLE:   Optimization_Interface/include/models/path_model_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Data model for vehicle trajectory

#ifndef PATH_MODEL_ITEM_H_
#define PATH_MODEL_ITEM_H_

#include <QPointF>
#include <QVector>
#include <QMutex>

#include "include/models/data_model.h"

namespace optgui {

class PathModelItem : public DataModel {
 public:
    PathModelItem() : DataModel(), mutex_() {
    }

    ~PathModelItem() {
        // acquire lock to destroy it
        QMutexLocker locker(&this->mutex_);
    }

    quint32 getSize() {
        QMutexLocker locker(&this->mutex_);
        // get number of points in traj
        return this->points_.size();
    }

    void setPointAt(QPointF point, int index) {
        QMutexLocker locker(&this->mutex_);
        // set point in traj if within bounds
        if (index < this->points_.size()) {
            QPointF &temp = this->points_[index];
            temp.setX(point.x());
            temp.setY(point.y());
        }
    }

    QPointF getPointAt(int index) {
        QMutexLocker locker(&this->mutex_);
        // get point in traj if within bounds,
        // return default point if out of bounds
        if (index < this->points_.size()) {
            return this->points_.value(index);
        } else {
            return QPointF();
        }
    }

    void setPoints(QVector<QPointF> points) {
        QMutexLocker locker(&this->mutex_);
        // copy over points
        this->points_ = points;
    }

    void addPoint(QPointF point) {
        QMutexLocker locker(&this->mutex_);
        // append point to traj
        this->points_.append(point);
    }

    void removePointAt(int index) {
        QMutexLocker locker(&this->mutex_);
        // remove point from traj if within bounds
        if (index < this->points_.size()) {
            this->points_.removeAt(index);
        }
    }

    void clearPoints() {
        QMutexLocker locker(&this->mutex_);
        this->points_.clear();
    }

    QVector<QPointF> getPoints() {
        QMutexLocker locker(&this->mutex_);
        // get copy of points
        return this->points_;
    }

 private:
    // mutex lock for getters/setters
    QMutex mutex_;
    // point in trajectory
    QVector<QPointF> points_;
};

}  // namespace optgui

#endif  // PATH_MODEL_ITEM_H_
