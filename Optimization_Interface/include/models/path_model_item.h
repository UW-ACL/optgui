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

namespace optgui {

class PathModelItem : public DataModel {
 public:
    explicit PathModelItem() : mutex_()
        { this->points_ = QVector<QPointF>(); port_ = 0; }
    ~PathModelItem() {
        QMutexLocker locker(&this->mutex_);
    }

    quint32 getSize() {
        QMutexLocker locker(&this->mutex_);
        return this->points_.size();
    }

    void setPointAt(QPointF point, int index) {
        QMutexLocker locker(&this->mutex_);
        if (index < this->points_.size()) {
            QPointF &temp = this->points_[index];
            temp.setX(point.x());
            temp.setY(point.y());
        }
    }

    QPointF getPointAt(int index) {
        QMutexLocker locker(&this->mutex_);
        QPointF temp = QPointF();
        if (index < this->points_.size()) {
            temp = this->points_.value(index);
        }
        return temp;
    }

    void setPoints(QVector<QPointF> points) {
        QMutexLocker locker(&this->mutex_);
        this->points_ = points;
    }

    void addPoint(QPointF point) {
        QMutexLocker locker(&this->mutex_);
        this->points_.append(point);
    }

    void removePointAt(int index) {
        QMutexLocker locker(&this->mutex_);
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
        return this->points_;
    }

 private:
    QMutex mutex_;
    QVector<QPointF> points_;
};

}  // namespace optgui

#endif  // PATH_MODEL_ITEM_H_
