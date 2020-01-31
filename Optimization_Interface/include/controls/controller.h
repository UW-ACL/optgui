// TITLE:   Optimization_Interface/include/controls/controller.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Controller for modifying Model and Viewing components

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

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
#include "include/network/drone_socket.h"
#include "include/network/ellipse_socket.h"
#include "include/network/point_socket.h"
#include "include/window/menu_panel.h"

namespace interface {

class Controller : public QObject {
    Q_OBJECT

 public:
    explicit Controller(Canvas *canvas, MenuPanel *menupanel);
    ~Controller();

    void setCanvas(Canvas *canvas);  // sets up canvas for drawing graphics

    double solver_difficulty_ = 100;

    bool valid_path_ = false;
    bool indoor_ = true;
    ConstraintModel *model_;

    // add constraints
    void addEllipse(QPointF *point, qreal radius = 120);
    void addPolygon(QVector<QPointF *> *points);
    void addPlane(QPointF *p1, QPointF *p2);

    // flip constraint direction
    void flipDirection(QGraphicsItem *item);

    // functions to add points for vehicle, obstacle, and waypoint locations
    void addWaypoint(QPointF *point);
    void addPathPoint(QPointF *point);
    void clearPathPoints();
    void removeAllWaypoints();
    void removeItem(QGraphicsItem *item);
    void duplicateSelected();

    // save/load functionality
    void loadFile();
    void saveFile();

    // network functionality
    void setPorts();

    // functions for setting optimization problem constraints
    void setFinaltime(double);
    void setHorizonLength(uint32_t);
    void updateFinalPosition(QPointF const &pos);
    double getTimeInterval();

    // functions for computing, simulating and executing trajectories
    void execute();
    void updatePath();
    bool simDrone(uint64_t tick);
    bool isFrozen();

 signals:
    void trajectoryExecuted(const autogen::packet::traj3dof *data);
    void startCompute();

 private slots:
    void startSockets();
//    void compute(QVector<QPointF *> *trajectory);
    void compute();

 private:
    // QGraphicsScene
    Canvas *canvas_;
    // Side menu panel
    MenuPanel *menu_panel_;

    // Port setting dialog and network sockets
    PortDialog *port_dialog_;
    DroneSocket *drone_socket_;
    PointSocket *final_point_socket_;
    QVector<EllipseSocket *> *ellipse_sockets_;

    void removeEllipseSocket(EllipseModelItem *model);
    void closeSockets();

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
