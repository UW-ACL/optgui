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

namespace optgui {

class PolygonModelItem : public DataModel {
 public:
    explicit PolygonModelItem(QVector<QPointF> points) : DataModel(),
        mutex_(), direction_(false) {
        // initialize from copy of points param
        points_ = points;
    }

    ~PolygonModelItem() {
        // acquire lock to destroy it
        QMutexLocker locker(&this->mutex_);
    }

    quint32 getSize() {
        QMutexLocker locker(&this->mutex_);
        // get number of verticies
        return this->points_.size();
    }

    void setPointAt(QPointF point, quint32 index) {
        QMutexLocker locker(&this->mutex_);
        // set point of vertex without bounds checking
        QPointF &temp = this->points_[index];
        temp.setX(point.x());
        temp.setY(point.y());
    }

    QPointF getPointAt(quint32 index) {
        QMutexLocker locker(&this->mutex_);
        // get copy of point vertex in xyz pixels
        return this->points_.value(index);
    }

    bool getDirection() {
        QMutexLocker locker(&this->mutex_);
        // get direction of constraint inequality
        return this->direction_;
    }

    void flipDirection() {
        QMutexLocker locker(&this->mutex_);
        // flip direction of constraint inequality
        this->direction_ = !this->direction_;
    }

    bool isConvex() {
        QMutexLocker locker(&this->mutex_);

        // detect whenther polygon is convex by summing inner angles
        // CURRENTLY DOES NOT CHECK FOR SELF INTERSECTING POLYGONS
        quint32 n = this->points_.size();
        if (n < 4) {
            return true;
        }

        bool sign = false;

        for (quint32 i = 0; i < n; i++) {
            QPointF p0 = this->points_.at(i);
            QPointF p1 = this->points_.at((i + 1) % n);
            QPointF p2 = this->points_.at((i + 2) % n);
            qreal dx1 = p2.x() - p1.x();
            qreal dy1 = p2.y() - p1.y();
            qreal dx2 = p0.x() - p1.x();
            qreal dy2 = p0.y() - p1.y();
            qreal z_cross_product = (dx1 * dy2) - (dy1 * dx2);

            if (i == 0) {
                sign = z_cross_product > 0;
            } else if (sign != (z_cross_product > 0)) {
                return false;
            }
        }
        return true;
    }

 private:
    QMutex mutex_;
    QVector<QPointF> points_;
    bool direction_;
};

}  // namespace optgui

#endif  // POLYGON_MODEL_ITEM_H_
