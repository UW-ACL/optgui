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
    PathModelItem() : mutex_()
        { this->points_ = QVector<QPointF>(); port_ = 0; }
    ~PathModelItem() {
        this->mutex_.lock();
        this->mutex_.unlock();
    }

    quint32 getSize() {
        this->mutex_.lock();
        quint32 size = this->points_.size();
        this->mutex_.unlock();
        return size;
    }

    void setPointAt(QPointF point, quint32 index) {
        this->mutex_.lock();
        if (index < this->points_.size()) {
            QPointF &temp = this->points_[index];
            temp.setX(point.x());
            temp.setY(point.y());
        }
        this->mutex_.unlock();
    }

    QPointF getPointAt(quint32 index) {
        this->mutex_.lock();
        QPointF temp = QPointF();
        if (index < this->points_.size()) {
            temp = this->points_.value(index);
        }
        this->mutex_.unlock();
        return temp;
    }

    void setPoints(QVector<QPointF> points) {
        this->mutex_.lock();
        this->points_ = points;
        this->mutex_.unlock();
    }

    void addPoint(QPointF point) {
        this->mutex_.lock();
        this->points_.append(point);
        this->mutex_.unlock();
    }

    void removePointAt(quint32 index) {
        this->mutex_.lock();
        if (index < this->points_.size()) {
            this->points_.removeAt(index);
        }
        this->mutex_.unlock();
    }

    void clearPoints() {
        this->mutex_.lock();
        this->points_.clear();
        this->mutex_.unlock();
    }

    QVector<QPointF> getPoints() {
        this->mutex_.lock();
        QVector<QPointF> temp = this->points_;
        this->mutex_.unlock();
        return temp;
    }

 private:
    QMutex mutex_;
    QVector<QPointF> points_;
};

}  // namespace optgui

#endif  // PATH_MODEL_ITEM_H_
