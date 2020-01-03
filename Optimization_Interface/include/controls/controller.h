// TITLE:   Optimization_Interface/include/controls/controller.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Controller for modifying Model and Viewing components

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <QNetworkSession>
#include <QElapsedTimer>

#include <cprs.h>
#include <algorithm.h>

#include "include/graphics/canvas.h"
#include "include/models/constraint_model.h"
#include "include/graphics/path_graphics_item.h"
#include "include/graphics/drone_graphics_item.h"
#include "include/graphics/waypoints_graphics_item.h"
#include "include/graphics/point_graphics_item.h"
#include "include/window/port_dialog.h"
#include "include/network/item_server.h"
#include "include/network/comm.h"
#include "include/window/menu_panel.h"

namespace interface {

class Controller : public QObject {
    Q_OBJECT

 public:
    explicit Controller(Canvas *canvas, MenuPanel *menupanel);
    ~Controller();

    void setCanvas(Canvas *canvas);  // sets up canvas for drawing graphics

    // Controller parameters
    // TODO(bchasnov): make a proper class for these parameters
    float_t finaltime_;
    uint32_t horizon_length_ = MAX_HORIZON;

    comm *drone_comm_;
    uint32_t drone_port_ = 8000;

    // TODO(bchasnov): update puck to marker with feedback
    double solver_difficulty_ = 100;

    bool valid_path_ = false;
    bool indoor_ = true;
    ConstraintModel *model_;

    // functions to add constraints
    void addEllipse(QPointF *point, qreal radius = 120);
    void addPolygon(QVector<QPointF *> *points);
    void addPlane(QPointF *p1, QPointF *p2);
    void flipDirection(QGraphicsItem *item);

    // functions to add points for vehicle, obstacle, and waypoint locations
    void addWaypoint(QPointF *point);
    void addPathPoint(QPointF *point);
    void clearPathPoints();
    void clearPathGraphic();
    void removeAllWaypoints();
    void clearWaypointsGraphic();
    void removeItem(QGraphicsItem *item);
    void updateDronePos(QPointF pos);
    void clearDroneGraphic();
    void updatePuckPos(uint32_t idx, QPointF pos);
    // TODO(bchasnov): Add function to clear puck graphic

    // functions for loading and saving files
    void loadFile();
    void saveFile();
    void setPorts();
    void startServers();
    void closeServers();

    // functions for setting optimization problem constraints
    void setFinaltime(double_t);
    void setHorizonLength(uint32_t);
    void updateFinalPosition(QPointF const &pos);
    double_t getTimeInterval();

    // functions for computing, simulating and executing trajectories
    void compute();
    void compute(QVector<QPointF* > *trajectory);
    void execute();
    void updatePath();
    bool simDrone(uint64_t tick);
    bool isFrozen();

 signals:
    void trajectoryExecuted(const autogen::packet::traj3dof* data);

 private:
    Canvas *canvas_;
    MenuPanel *menu_panel_;

    // TODO(dtsull): move these graphics to the canvas
    WaypointsGraphicsItem *waypoints_graphic_;
    PathGraphicsItem *path_graphic_;
    DroneGraphicsItem *drone_graphic_;

    PortDialog *port_dialog_;
    QNetworkSession *network_session_;
    QVector<ItemServer *> *servers_;

    PointGraphicsItem *final_point_;

    // TODO(bchasnov): remove these terrible null pointer......
    QVector<QPointF *>* trajectory_;
    double feasible_tol_ = pow(0.5, 2);

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

    QElapsedTimer timer_exec_;
    QElapsedTimer timer_compute_;

    bool exec_once_ = false;

    autogen::packet::traj3dof drone_traj3dof_data_;
};

}  // namespace interface

#endif  // CONTROLLER_H_