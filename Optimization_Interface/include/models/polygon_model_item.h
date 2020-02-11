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
    explicit PolygonModelItem(QVector<QPointF> points) :
        mutex_(), direction_(false) { points_ = points; port_ = 0;}
    ~PolygonModelItem() {
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
        QPointF &temp = this->points_[index];
        temp.setX(point.x());
        temp.setY(point.y());
        this->mutex_.unlock();
    }

    QPointF getPointAt(quint32 index) {
        this->mutex_.lock();
        QPointF temp = this->points_.value(index);
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

    bool isConvex() {
        this->mutex_.lock();
        quint32 n = this->points_.size();
        if (n < 4) {
            this->mutex_.unlock();
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
                this->mutex_.unlock();
                return false;
            }
        }
        this->mutex_.unlock();
        return true;
    }

 private:
    QMutex mutex_;
    QVector<QPointF> points_;
    bool direction_;
};

}  // namespace optgui

#endif  // POLYGON_MODEL_ITEM_H_
