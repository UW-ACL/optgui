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
    this->ellipses_ = new QSet<EllipseModelItem *>();
    this->polygons_ = new QSet<PolygonModelItem *>();
    this->planes_ = new QSet<PlaneModelItem *>();
    this->path_ = new QVector<QPointF *>();
    this->course_ = new QVector<QPointF *>();
    this->drone_ = new QPointF(0, 0);
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

    // Delete path points
    for (QPointF *point : *this->path_) {
        delete point;
    }
    delete this->path_;

    // Delete course points
    for (QPointF *point : *this->course_) {
        delete point;
    }
    delete this->course_;

    // Delete drone
    delete this->drone_;
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

void ConstraintModel::addPathPoint(QPointF *item) {
    this->path_->append(item);
}

void ConstraintModel::removePathPoint(QPointF *item) {
    this->path_->removeOne(item);
}

void ConstraintModel::addCoursePoint(QPointF *item) {
    this->course_->append(item);
}

void ConstraintModel::clearCourse() {
    this->course_->clear();
}

}  // namespace interface
