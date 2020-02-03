// TITLE:   Optimization_Interface/src/models/constraint_model.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/models/constraint_model.h"

#include <QString>
#include <QLineF>

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
    this->final_pos_ = new PointModelItem();
    this->ellipses_ = new QSet<EllipseModelItem *>();
    this->polygons_ = new QSet<PolygonModelItem *>();
    this->planes_ = new QSet<PlaneModelItem *>();
    this->waypoints_ = new PathModelItem();
    this->path_ = new PathModelItem();
    this->drone_ = new DroneModelItem();

    // initialize algorithm
    this->finaltime_ = 3;
    this->horizon_length_ = 16;
  
    // trajectory points
    this->trajectory_ = new QVector<QPointF *>;
    this->initializeFly();
}

ConstraintModel::~ConstraintModel() {
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

void ConstraintModel::addEllipse(EllipseModelItem *item) {
    this->ellipses_->insert(item);
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

uint32_t ConstraintModel::loadEllipseConstraint(double* R, double* c_e, double* c_n) {
    uint32_t index = 0;
    for (EllipseModelItem *ellipse : *this->ellipses_) {
        R[index] = ellipse->radius_/this->scale_;
        c_e[index] = -ellipse->pos_->y()/this->scale_;
        c_n[index] = ellipse->pos_->x()/this->scale_;
        index++;
        if (index >= this->maxEllipse) return index;
    }
    return index;
}

bool ConstraintModel::isEllipseOverlap(QPointF * pos) {
    for (EllipseModelItem *ellipse : *this->ellipses_) {
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

uint32_t ConstraintModel::loadPosConstraints(double* A, double* b) {
    uint32_t index = 0;

    for (PolygonModelItem *polygon : *this->polygons_) {
        for (qint32 i = 1; i < polygon->points_->length() + 1; i++) {
            QPointF *p = polygon->points_->at(i - 1);
            QPointF *q = polygon->points_->at(i % polygon->points_->length());
            if (polygon->direction_) {
                this->loadPlaneConstraint(A, b, index, q, p);
            } else {
                this->loadPlaneConstraint(A, b, index, p, q);
            }
            index++;
            if (index >= this->maxHalfspace) return index;
        }
    }

    for (PlaneModelItem *plane : *this->planes_) {
        QPointF *p = plane->p1_;
        QPointF *q = plane->p2_;

        if (plane->direction_) {
            this->loadPlaneConstraint(A, b, index, q, p);
        } else {
            this->loadPlaneConstraint(A, b, index, p, q);
        }
        index++;
        if (index >= this->maxHalfspace) return index;
    }

    return index;
}

void ConstraintModel::loadPlaneConstraint(double *A, double *b, uint32_t index,
                                              QPointF *p, QPointF *q) {
    qreal px = -p->y() / this->scale_;
    qreal py = p->x() / this->scale_;
    qreal qx = -q->y() / this->scale_;
    qreal qy = q->x() / this->scale_;
    qreal c = ((py * qx) - (px * qy));

    qreal a1 = (py - qy) / c;
    qreal a2 = (px - qx) / -c;

    QLineF line(*p, *q);
    QPointF normal = line.normalVector().p2();
    qreal flip = ((a1 * normal.x()) + (a2 * normal.y()) < 1) ? 1 : -1;

    A[2 * index] = flip * a1;
    A[(2 * index) + 1] = flip * a2;
    b[index] = flip;
}

}  // namespace interface


