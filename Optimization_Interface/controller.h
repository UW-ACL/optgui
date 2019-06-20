// TITLE:   Optimization_Interface/controller.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Controller for modifying Model and Viewing components

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <QNetworkSession>

#include "canvas.h"
#include "constraint_model.h"
#include "path_graphics_item.h"
#include "drone_graphics_item.h"
#include "waypoints_graphics_item.h"
#include "point_graphics_item.h"
#include "port_dialog.h"
#include "item_server.h"
#include "cprs.h"
#include "comm.h"

namespace interface {

class Controller {
 public:
    explicit Controller(Canvas *canvas);
    ~Controller();

    void setCanvas(Canvas *canvas);

    void addEllipse(QPointF *point);
    void addPolygon(QVector<QPointF *> *points);
    void addPlane(QPointF *p1, QPointF *p2);
    void addWaypoint(QPointF *point);
    void addPathPoint(QPointF *point);
    void clearPathPoints();
    void updateDronePos(QPointF pos);

    void removeAllWaypoints();

    void removeItem(QGraphicsItem *item);
    void flipDirection(QGraphicsItem *item);

    void loadFile();
    void saveFile();
    void setPorts();
    void startServers();
    void closeServers();
    void compute();
    void compute(QVector<QPointF* > *trajectory);
    void execute();

    void setFinaltime(double_t);
    void setHorizonLength(uint32_t);
    void updateFinalPosition(QPointF *);
    double_t getTimeInterval();

    void updatePath();

    void clearWaypointsGraphic();
    void clearPathGraphic();
    void clearDroneGraphic();

    bool simDrone(uint64_t tick);

    // TODO: make a proper class for these parameters
    uint32_t horizon_length_ = 20;
    double_t finaltime_ = 2.7;
    uint32_t drone_port_ = 8000;
    uint32_t puck_port_ = 8001;

    comm *drone_comm_;
    comm *puck_comm_;

 private:
    Canvas *canvas_;
    ConstraintModel *model_;
    WaypointsGraphicsItem *waypoints_graphic_;
    PathGraphicsItem *path_graphic_;
    DroneGraphicsItem *drone_graphic_;
    PortDialog *port_dialog_;
    QNetworkSession *network_session_;
    QVector<ItemServer *> *servers_;

    PointGraphicsItem *final_pos_graphic_;

    // TODO: remove these terrible null pointer......
    QVector<QPointF *>* trajectory_;



    void loadPoint(PointModelItem *model);
    void loadEllipse(EllipseModelItem *model);
    void loadPolygon(PolygonModelItem *model);
    void loadPlane(PlaneModelItem *model);

    void writePoint(PointModelItem *model, QDataStream *out);
    void writeEllipse(EllipseModelItem *model, QDataStream *out);
    void writePolygon(PolygonModelItem *model, QDataStream *out);
    void writePlane(PlaneModelItem *model, QDataStream *out);
    void writeWaypoints(PathModelItem *model, QDataStream *out);
    void writePath(PathModelItem *model, QDataStream *out);
    void writeDrone(DroneModelItem *model, QDataStream *out);


    PointModelItem *readPoint(QDataStream *in);
    EllipseModelItem *readEllipse(QDataStream *in);
    PolygonModelItem *readPolygon(QDataStream *in);
    PlaneModelItem *readPlane(QDataStream *in);
    void readWaypoints(QDataStream *in);
    void readPath(QDataStream *in);
    void readDrone(QDataStream *in);
};

}  // namespace interface

#endif  // CONTROLLER_H_
