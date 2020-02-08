// TITLE:   Optimization_Interface/src/models/constraint_model.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/models/constraint_model.h"

#include <QString>
#include <QLineF>

namespace interface {

ConstraintModel::ConstraintModel() : model_lock_() {
    this->initialize();
}

void ConstraintModel::initialize() {
    // Set model containers
    this->ellipses_ = new QSet<EllipseModelItem *>();
    this->polygons_ = new QSet<PolygonModelItem *>();
    this->planes_ = new QSet<PlaneModelItem *>();
    this->final_pos_ = nullptr;
    this->waypoints_ = nullptr;
    this->path_ = nullptr;
    this->drone_ = nullptr;

    // initialize algorithm variables
    this->finaltime_ = 3;
    this->horizon_length_ = 16;
}

ConstraintModel::~ConstraintModel() {
    this->model_lock_.lock();
    // Delete ellipses
    for (EllipseModelItem *ellipse : *this->ellipses_) {
        delete ellipse;
    }
    delete this->ellipses_;

    // Delete polygons
    for (PolygonModelItem *polygon : *this->polygons_) {
        delete polygon;
    }
    delete this->polygons_;

    // Delete planes
    for (PlaneModelItem *plane : *this->planes_) {
        delete plane;
    }
    delete this->planes_;

    // Delete waypoints
    if (!this->waypoints_) {
        delete this->waypoints_;
    }
    // Delete path
    if (!this->path_) {
        delete this->path_;
    }
    // Delete drone
    if (!this->drone_) {
        delete this->drone_;
    }

    // Delete final point
    if (!this->final_pos_) {
        delete this->final_pos_;
    }
    this->model_lock_.unlock();
}

// Public functions, need to lock

void ConstraintModel::addEllipse(EllipseModelItem *item) {
    this->model_lock_.lock();
    this->ellipses_->insert(item);
    this->model_lock_.unlock();
}

void ConstraintModel::removeEllipse(EllipseModelItem *item) {
    this->model_lock_.lock();
    this->ellipses_->remove(item);
    this->model_lock_.unlock();
}

void ConstraintModel::addPolygon(PolygonModelItem *item) {
    this->model_lock_.lock();
    this->polygons_->insert(item);
    this->model_lock_.unlock();
}

void ConstraintModel::removePolygon(PolygonModelItem *item) {
    this->model_lock_.lock();
    this->polygons_->remove(item);
    this->model_lock_.unlock();
}

void ConstraintModel::addPlane(PlaneModelItem *item) {
    this->model_lock_.lock();
    this->planes_->insert(item);
    this->model_lock_.unlock();
}

void ConstraintModel::removePlane(PlaneModelItem *item) {
    this->model_lock_.lock();
    this->planes_->remove(item);
    this->model_lock_.unlock();
}

void ConstraintModel::setWaypointsModel(PathModelItem *waypoints) {
    this->model_lock_.lock();
    if (!this->waypoints_) {
        delete this->waypoints_;
    }
    this->waypoints_ = waypoints;
    this->model_lock_.unlock();
}

void ConstraintModel::addWaypoint(QPointF *item) {
    this->model_lock_.lock();
    this->waypoints_->addPoint(item);
    this->model_lock_.unlock();
}

void ConstraintModel::removeWaypoint(QPointF *item) {
    this->model_lock_.lock();
    this->waypoints_->removePoint(item);
    this->model_lock_.unlock();
}

void ConstraintModel::setPathModel(PathModelItem *trajectory) {
    this->model_lock_.lock();
    if (!this->path_) {
        delete this->path_;
    }
    this->path_ = trajectory;
    this->model_lock_.unlock();
}

void ConstraintModel::setPathPoints(QVector<QPointF *> *points) {
    this->model_lock_.lock();
    this->path_->setPoints(points);
    this->model_lock_.lock();
}

void ConstraintModel::setDroneModel(DroneModelItem *drone) {
    this->model_lock_.lock();
    if (!this->drone_) {
        delete this->drone_;
    }
    this->drone_ = drone;
    this->model_lock_.unlock();
}

void ConstraintModel::setFinalPointModel(PointModelItem *final_point) {
    this->model_lock_.lock();
    if (!this->final_pos_) {
        delete this->final_pos_;
    }
    this->final_pos_ = final_point;
    this->model_lock_.unlock();
}

void ConstraintModel::loadFinalPos(double* r_f) {
    this->model_lock_.lock();
    QPointF pos = this->final_pos_->getPos();
    r_f[1] = -pos.y() / GRID_SIZE;
    r_f[2] = pos.x() / GRID_SIZE;
    this->model_lock_.unlock();
}

void ConstraintModel::loadInitialPos(double* r_i) {
    this->model_lock_.lock();
    QPointF pos = this->drone_->getPos();
    r_i[1] = -pos.y() / GRID_SIZE;
    r_i[2] = pos.x() / GRID_SIZE;
    this->model_lock_.unlock();
}

uint32_t ConstraintModel::loadEllipseConstraints(double* R, double* c_e, double* c_n) {
    uint32_t index = 0;
    this->model_lock_.lock();
    for (EllipseModelItem *ellipse : *this->ellipses_) {
        R[index] = ellipse->getRadius() / GRID_SIZE;
        QPointF pos = this->drone_->getPos();
        c_e[index] = -pos.y() / GRID_SIZE;
        c_n[index] = pos.x() / GRID_SIZE;
        index++;
        if (index >= MAX_OBS) {
            this->model_lock_.unlock();
            return index;
        }
    }
    this->model_lock_.unlock();
    return index;
}

uint32_t ConstraintModel::loadPosConstraints(double* A, double* b) {
    uint32_t index = 0;
    this->model_lock_.lock();
    for (PolygonModelItem *polygon : *this->polygons_) {
        uint32_t size = polygon->getSize();
        for (qint32 i = 1; i < size + 1; i++) {
            QPointF p = polygon->getPointAt(i - 1);
            QPointF q = polygon->getPointAt(i % size);
            if (polygon->getDirection()) {
                this->loadPlaneConstraint(A, b, index, q, p);
            } else {
                this->loadPlaneConstraint(A, b, index, p, q);
            }
            index++;
            if (index >= MAX_CPOS) {
                this->model_lock_.unlock();
                return index;
            }
        }
    }

    for (PlaneModelItem *plane : *this->planes_) {
        QPointF p = plane->getP1();
        QPointF q = plane->getP2();

        if (plane->getDirection()) {
            this->loadPlaneConstraint(A, b, index, q, p);
        } else {
            this->loadPlaneConstraint(A, b, index, p, q);
        }
        index++;
        if (index >= MAX_CPOS) {
            this->model_lock_.unlock();
            return index;
        }
    }
    this->model_lock_.unlock();
    return index;
}

qreal ConstraintModel::getFinaltime() {
    this->model_lock_.lock();
    qreal temp = this->finaltime_;
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setFinaltime(qreal finaltime) {
    this->model_lock_.lock();
    this->finaltime_ = finaltime;
    this->model_lock_.unlock();
}

uint32_t ConstraintModel::getHorizon() {
    this->model_lock_.lock();
    uint32_t temp = this->horizon_length_;
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setHorizon(uint32_t horizon) {
    this->model_lock_.lock();
    this->horizon_length_ = horizon;
    this->model_lock_.unlock();
}

// Private functions, should not lock

void ConstraintModel::loadPlaneConstraint(double *A, double *b, uint32_t index,
                                              QPointF p, QPointF q) {
    qreal px = -p.y() / GRID_SIZE;
    qreal py = p.x() / GRID_SIZE;
    qreal qx = -q.y() / GRID_SIZE;
    qreal qy = q.x() / GRID_SIZE;
    qreal c = ((py * qx) - (px * qy));

    qreal a1 = (py - qy) / c;
    qreal a2 = (px - qx) / -c;

    QLineF line(p, q);
    QPointF normal = line.normalVector().p2();
    qreal flip = ((a1 * normal.x()) + (a2 * normal.y()) < 1) ? 1 : -1;

    A[2 * index] = flip * a1;
    A[(2 * index) + 1] = flip * a2;
    b[index] = flip;
}

}  // namespace interface


