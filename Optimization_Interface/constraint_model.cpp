// TITLE:   Optimization_Interface/constraint_model.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "constraint_model.h"
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
    this->puck_pos_ = new QVector<PointModelItem *>();
    this->puck_pos_->append(new PointModelItem());
    this->ellipses_ = new QSet<EllipseModelItem *>();
    this->polygons_ = new QSet<PolygonModelItem *>();
    this->planes_ = new QSet<PlaneModelItem *>();
    this->waypoints_ = new PathModelItem();
    this->path_ = new PathModelItem();
    this->drone_ = new DroneModelItem();
    QSet<EllipseModelItem * > set;
    QSetIterator<EllipseModelItem *> i(set);

}

ConstraintModel::~ConstraintModel() {
    // Delete points
    for (PointModelItem *point : *this->points_) {
        delete point;
    }
    delete this->ellipses_;
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
}

void ConstraintModel::addPoint(PointModelItem *item) {
    this->points_->insert(item);
}

void ConstraintModel::updateInitialPoint(QPointF *item) {
    // TODO:
}
void ConstraintModel::updateFinalPoint(QPointF *item) {
    // TODO:
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
    r_f[1] = this->final_pos_->pos_->x()/this->scale_;
    r_f[2] = this->final_pos_->pos_->y()/this->scale_;
}

void ConstraintModel::loadInitialPos(double* r_i) {
    r_i[1] = this->drone_->point_->x()/this->scale_;
    r_i[2] = this->drone_->point_->y()/this->scale_;
}

uint32_t ConstraintModel::loadEllipse(double* R, double* c_e, double* c_n) {
    uint32_t j = 0;
    QSetIterator<EllipseModelItem *> iter(*this->ellipses_);
    while(iter.hasNext()) {
        if (j >= this->maxEllipse) break;

        EllipseModelItem* ellipse = iter.next();
        R[j] = ellipse->radius_/this->scale_ + ellipse->clearance_/this->scale_;
        c_e[j] = ellipse->pos_->x()/this->scale_;
        c_n[j] = ellipse->pos_->y()/this->scale_;
        ++j;
    }
    return j;
}

uint32_t ConstraintModel::loadPosConstraint(double* A, double* b) {
    uint32_t num = 0;

    for (PolygonModelItem *polygon : *this->polygons_) {
        for (qint32 i = 1; i < polygon->points_->length()+1; i++) {
            QPointF *p = polygon->points_->at(i-1);
            QPointF *q = polygon->points_->at(i % polygon->points_->length());
            int32_t flip = (polygon->direction_?-1:1);

            double px = p->x()/this->scale_;
            double py = p->y()/this->scale_;
            double qx = q->x()/this->scale_;
            double qy = q->y()/this->scale_;
            double c = (py*qx-px*qy);

            A[2*(i-1)] = flip*(py-qy)/c;
            A[2*(i-1)+1] = flip*(qx-px)/c;
            b[i-1] = flip;
            num++;
        }
    }

    for (PlaneModelItem *plane : *this->planes_) {
        uint32_t i = num+1;
        QPointF *p = plane->p1_;
        QPointF *q = plane->p2_;
        int32_t flip = (plane->direction_?1:-1);

        double px = p->x()/this->scale_;
        double py = p->y()/this->scale_;
        double qx = q->x()/this->scale_;
        double qy = q->y()/this->scale_;
        double c = (py*qx-px*qy);

        A[2*(i-1)] = flip*(py-qy)/c;
        A[2*(i-1)+1] = flip*(qx-px)/c;
        b[i-1] = flip;
        num++;

    }
    return num;
}

}  // namespace interface


