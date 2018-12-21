// TITLE:   Optimization_Interface/constraint_model.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef CONSTRAINT_MODEL_H
#define CONSTRAINT_MODEL_H

#include <QSet>
#include <QVector>
#include <QPointF>

#include "ellipse_model_item.h"
#include "polygon_model_item.h"
#include "plane_model_item.h"

namespace interface {

class ConstraintModel
{
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
    QVector<QPointF *> *getPathOrdering();
private:
    QSet<EllipseModelItem *> *ellipses_;
    QSet<PolygonModelItem *> *polygons_;
    QSet<PlaneModelItem *> *planes_;
    QVector<QPointF *> *path_;
    void initialize();
};

}  // namespace

#endif // CONSTRAINT_MODEL_H
