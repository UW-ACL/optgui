// TITLE:   Optimization_Interface/constraint_model.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "constraint_model.h"

namespace interface {

ConstraintModel::ConstraintModel() {
    this->initialize();
}

void ConstraintModel::initialize() {
    // Set model containers
    this->points_ = new QSet<PointModelItem *>();
    this->ellipses_ = new QSet<EllipseModelItem *>();
    this->polygons_ = new QSet<PolygonModelItem *>();
    this->planes_ = new QSet<PlaneModelItem *>();
    this->waypoints_ = new PathModelItem();
    this->path_ = new PathModelItem();
    this->drone_ = new DroneModelItem();
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

}  // namespace interface
