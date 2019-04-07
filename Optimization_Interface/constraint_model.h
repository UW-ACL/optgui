// TITLE:   Optimization_Interface/constraint_model.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Model holding data for all constraints

#ifndef CONSTRAINT_MODEL_H_
#define CONSTRAINT_MODEL_H_

#include <QSet>
#include <QVector>
#include <QPointF>

#include "ellipse_model_item.h"
#include "polygon_model_item.h"
#include "plane_model_item.h"

namespace interface {

class ConstraintModel {
 public:
    ConstraintModel();
    ~ConstraintModel();

    void addEllipse(EllipseModelItem *item);
    void removeEllipse(EllipseModelItem *item);

    void addPolygon(PolygonModelItem *item);
    void removePolygon(PolygonModelItem *item);

    void addPlane(PlaneModelItem *item);
    void removePlane(PlaneModelItem *item);

    void addPathPoint(QPointF *item);
    void removePathPoint(QPointF *item);

    void addCoursePoint(QPointF *item);
    void clearCourse();

    QSet<EllipseModelItem *> *ellipses_;
    QSet<PolygonModelItem *> *polygons_;
    QSet<PlaneModelItem *> *planes_;
    QVector<QPointF *> *path_;
    QVector<QPointF *> *course_;
    QPointF *drone_;

 private:
    void initialize();
};

}  // namespace interface

#endif  // CONSTRAINT_MODEL_H_
