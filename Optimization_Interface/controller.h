// TITLE:   Optimization_Interface/controller.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Controller for modifying Model and Viewing components

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "canvas.h"
#include "constraint_model.h"
#include "course_graphics_item.h"
#include "drone_graphics_item.h"
#include "path_graphics_item.h"

namespace interface {

class Controller {
 public:
    explicit Controller(Canvas *canvas);
    ~Controller();

    void setCanvas(Canvas *canvas);

    void addEllipse(QPointF *point);
    void addPolygon(QVector<QPointF *> *points);
    void addPlane(QPointF *p1, QPointF *p2);
    void addPath(QPointF *point);
    void addCourse(QPointF *point);

    void removeItem(QGraphicsItem *item);
    void flipDirection(QGraphicsItem *item);

    void loadFile();
    void saveFile();

    void execute();
    void clearPath();
    void clearCourse();
    void clearDrone();

 private:
    Canvas *canvas_;
    ConstraintModel *model_;
    PathGraphicsItem *path_graphic_;
    CourseGraphicsItem *course_graphic_;
    DroneGraphicsItem *drone_graphic_;

    void loadEllipse(EllipseModelItem *model);
    void loadPolygon(PolygonModelItem *model);
    void loadPlane(PlaneModelItem *model);
    void loadPath(QPointF *point);

    void writeEllipse(EllipseModelItem *model, QDataStream *out);
    void writePolygon(PolygonModelItem *model, QDataStream *out);
    void writePlane(PlaneModelItem *model, QDataStream *out);
    void writePath(QPointF *point, QDataStream *out);

    EllipseModelItem *readEllipse(QDataStream *in);
    PolygonModelItem *readPolygon(QDataStream *in);
    PlaneModelItem *readPlane(QDataStream *in);
    QPointF *readPath(QDataStream *in);
};

}  // namespace interface

#endif  // CONTROLLER_H_
