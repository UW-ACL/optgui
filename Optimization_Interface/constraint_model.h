// TITLE:   Optimization_Interface/constraint_model.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Model holding data for all constraints and item info

#ifndef CONSTRAINT_MODEL_H_
#define CONSTRAINT_MODEL_H_

#include <QSet>
#include <QVector>
#include <QPointF>
#include <QSetIterator>

#include "point_model_item.h"
#include "ellipse_model_item.h"
#include "polygon_model_item.h"
#include "plane_model_item.h"
#include "path_model_item.h"
#include "drone_model_item.h"

namespace interface {

class ConstraintModel {
 public:
    ConstraintModel();
    ConstraintModel(uint32_t maxEllipse, uint32_t maxAffine);

    ~ConstraintModel();

    void addPoint(PointModelItem *item);
    void removePoint(PointModelItem *item);

    void addEllipse(EllipseModelItem *item);
    void removeEllipse(EllipseModelItem *item);

    void addPolygon(PolygonModelItem *item);
    void removePolygon(PolygonModelItem *item);

    void addPlane(PlaneModelItem *item);
    void removePlane(PlaneModelItem *item);

    void addWaypoint(QPointF *item);
    void removeWaypoint(QPointF *item);

    void addPathPoint(QPointF *item);
    void clearPath();

    void updateInitialPoint(QPointF *item);
    void updateFinalPoint(QPointF *item);

    void loadFinalPos(double*);
    void loadInitialPos(double*);

    uint32_t loadEllipse(double* R, double* c_e, double* c_n);
    uint32_t loadPosConstraint(double* A, double* b);

    QSet<PointModelItem *> *points_;
    QSet<EllipseModelItem *> *ellipses_;
    QSet<PolygonModelItem *> *polygons_;
    QSet<PlaneModelItem *> *planes_;
    PathModelItem *waypoints_;
    PathModelItem *path_;
    DroneModelItem *drone_;
    PointModelItem *final_pos_;

    uint32_t maxEllipse;
    uint32_t maxHalfspace;

    // TODO: fix magic number.. use mapfromscene?
    double scale_ = 100;

 private:
    void initialize();
};

}  // namespace interface

#endif  // CONSTRAINT_MODEL_H_
