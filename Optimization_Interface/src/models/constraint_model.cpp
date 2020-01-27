// TITLE:   Optimization_Interface/src/models/constraint_model.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/models/constraint_model.h"

#include <QString>

namespace interface {

ConstraintModel::ConstraintModel(uint32_t maxEllipse, uint32_t maxHalfspace) {
    this->maxEllipse = maxEllipse;
    this->maxHalfspace = maxHalfspace;
    this->initialize();
}

ConstraintModel::ConstraintModel() {
    this->initialize();
}

void ConstraintModel::initialize() {
    // Set model containers
    this->points_ = new QSet<PointModelItem *>();
    this->final_pos_ = new PointModelItem();
    this->ellipses_ = new QSet<EllipseModelItem *>();
    this->polygons_ = new QSet<PolygonModelItem *>();
    this->planes_ = new QSet<PlaneModelItem *>();
    this->waypoints_ = new PathModelItem();
    this->path_ = new PathModelItem();
    this->drone_ = new DroneModelItem();

    // initialize algorithm
    this->initializeFly();
    this->finaltime_ = 3;
    this->horizon_length_ = 16;

    // TODO(dtsull): these seem unnecessary
    QSet<EllipseModelItem *> set;
    QSetIterator<EllipseModelItem *> i(set);
}

ConstraintModel::~ConstraintModel() {
    // Delete points
    for (PointModelItem *point : *this->points_) {
        delete point;
    }
    delete this->points_;

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
    delete this->waypoints_;

    // Delete path
    delete this->path_;

    // Delete drone
    delete this->drone_;

    // Delete SkyeFly
    delete this->fly_;
}


void ConstraintModel::initializeFly() {
    this->fly_ = new SkyeFly();

    // Relaxation variables
    this->fly_->P.dK = 1;
    this->fly_->P.n_recalcs = 14;
    this->fly_->P.g[0] = -9.81;
    this->fly_->P.g[1] = 0.0;
    this->fly_->P.g[2] = 0.0;
    this->fly_->P.a_min = 5.0;   // Minimum accel.
    this->fly_->P.a_max = 12.0;  // Maximum accel.
    this->fly_->P.theta_max = 45.0 * DEG2RAD;
    this->fly_->P.q_max = 0.0;

    this->fly_->P.max_iter = 10;
    this->fly_->P.Delta_i = 100.0;
    this->fly_->P.lambda = 1e2;
    this->fly_->P.alpha = 2.0;
    this->fly_->P.dL_tol = 1e-1;
    this->fly_->P.rho_0 = -1e-1;
    this->fly_->P.rho_1 = 0.25;
    this->fly_->P.rho_2 = 0.90;
    this->fly_->P.rirelax = 1000;
    this->fly_->P.rfrelax = 10;
}

void ConstraintModel::addPoint(PointModelItem *item) {
    this->points_->insert(item);
}

void ConstraintModel::addEllipse(EllipseModelItem *item) {
    this->ellipses_->insert(item);
}

void ConstraintModel::removePoint(PointModelItem *item) {
    this->points_->remove(item);
}

void ConstraintModel::removeEllipse(EllipseModelItem *item) {
    this->ellipses_->remove(item);
}

void ConstraintModel::addPolygon(PolygonModelItem *item) {
    this->polygons_->insert(item);
}

void ConstraintModel::removePolygon(PolygonModelItem *item) {
    this->polygons_->remove(item);
}

void ConstraintModel::addPlane(PlaneModelItem *item) {
    this->planes_->insert(item);
}

void ConstraintModel::removePlane(PlaneModelItem *item) {
    this->planes_->remove(item);
}

void ConstraintModel::addWaypoint(QPointF *item) {
    this->waypoints_->points_->append(item);
}

void ConstraintModel::removeWaypoint(QPointF *item) {
    this->waypoints_->points_->removeOne(item);
}

void ConstraintModel::addPathPoint(QPointF *item) {
    this->path_->points_->append(item);
}

void ConstraintModel::clearPath() {
    this->path_->points_->clear();
}

void ConstraintModel::loadFinalPos(double* r_f) {
    r_f[1] = -this->final_pos_->pos_->y()/this->scale_;
    r_f[2] = this->final_pos_->pos_->x()/this->scale_;
}

void ConstraintModel::loadInitialPos(double* r_i) {
    r_i[1] = -this->drone_->pos_->y()/this->scale_;
    r_i[2] = this->drone_->pos_->x()/this->scale_;
}

uint32_t ConstraintModel::loadEllipse(double* R, double* c_e, double* c_n) {
    // TODO(dtsull): put this in a for loop
    uint32_t j = 0;
    QSetIterator<EllipseModelItem *> iter(*this->ellipses_);
    while (iter.hasNext()) {
        if (j >= this->maxEllipse) break;

        EllipseModelItem* ellipse = iter.next();
        R[j] = ellipse->radius_/this->scale_;
        c_e[j] = -ellipse->pos_->y()/this->scale_;
        c_n[j] = ellipse->pos_->x()/this->scale_;
        ++j;
    }
    return j;
}

bool ConstraintModel::isEllipseOverlap(QPointF * pos) {
    // TODO(dtsull): also put this in a for loop
    QSetIterator<EllipseModelItem *> iter(*this->ellipses_);
    while (iter.hasNext()) {
        EllipseModelItem* ellipse = iter.next();
        double r = ellipse->radius_/this->scale_;
        double dist = pow(pos->x() - ellipse->pos_->x(), 2)
                    + pow(pos->y() - ellipse->pos_->y(), 2);
        dist /= pow(this->scale_, 2);
        // DEBUG // fix this hardcoded costant...
        double dist_other = pow(r + 0.32, 2);
//        qDebug() << "Distance" << dist << "<=" << dist_other;
//        qDebug() << "r" << r << ", default rad" << DEFAULT_RAD;
        if (dist <= dist_other) {
            return true;
        }
    }
    return false;
}

uint32_t ConstraintModel::loadPosConstraint(double* A, double* b) {
    uint32_t num = 0;

    for (PolygonModelItem *polygon : *this->polygons_) {
        for (qint32 i = 1; i < polygon->points_->length() + 1; i++) {
            QPointF *p = polygon->points_->at(i - 1);
            QPointF *q = polygon->points_->at(i % polygon->points_->length());
            int32_t flip = (polygon->direction_?-1:1);

            qreal px = -p->y() / this->scale_;
            qreal py = p->x() / this->scale_;
            qreal qx = -q->y() / this->scale_;
            qreal qy = q->x() / this->scale_;
            qreal c = ((py * qx) - (px * qy));

            A[2 * (i - 1)] = (static_cast<double>(flip) * (py - qy) / c);
            A[2 * (i - 1) + 1] = (static_cast<double>(flip) * (qx - px) / c);
            b[i - 1] = static_cast<double>(flip);
            num++;
        }
    }

    for (PlaneModelItem *plane : *this->planes_) {
        uint32_t i = num+1;
        QPointF *p = plane->p1_;
        QPointF *q = plane->p2_;
        int32_t flip = (plane->direction_?1:-1);

        qreal px = -p->y() / this->scale_;
        qreal py = p->x() / this->scale_;
        qreal qx = -q->y() / this->scale_;
        qreal qy = q->x() / this->scale_;
        qreal c = ((py * qx) - (px * qy));

        A[2 * (i - 1)] = flip * (py - qy) / c;
        A[2 * (i - 1) + 1] = flip * (qx - px) / c;
        b[i - 1] = flip;
        num++;
    }
    return num;
}

}  // namespace interface


