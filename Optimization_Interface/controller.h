// TITLE:   Optimization_Interface/controller.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "canvas.h"
#include "constraint_model.h"

namespace interface {

class Controller
{
public:
    explicit Controller(Canvas *canvas);
    ~Controller();

    void addEllipse(QPointF *point);
    void addPolygon(QVector<QPointF *> *points);
    void addPlane(QPointF *p1, QPointF *p2);
    void addPath(QPointF *point);

    void removeItem(QGraphicsItem *item);
    void flipConvex(QGraphicsItem *item);
private:
    Canvas *canvas_;
    ConstraintModel *model_;
};

}  // namespace

#endif // CONTROLLER_H
