// TITLE:   Optimization_Interface/include/controls/controller.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Controller for modifying Model and Viewing components

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <QThread>

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
#include "include/controls/compute_worker.h"

namespace interface {

class Controller : public QObject {
    Q_OBJECT

 public:
    explicit Controller(Canvas *canvas, MenuPanel *menupanel);
    ~Controller();

    void setCanvas(Canvas *canvas);  // sets up canvas for drawing graphics

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
    void startComputeWorker();

 private slots:
    void startSockets();
    void threadExitDebug();

 private:
    // QGraphicsScene
    Canvas *canvas_;
    // Side menu panel
    MenuPanel *menu_panel_;

    // SkyFly compute thread
    QThread compute_thread_;

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
};

}  // namespace interface

#endif  // CONTROLLER_H_
