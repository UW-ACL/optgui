// TITLE:   Optimization_Interface/include/controls/controller.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Controller for modifying Model and Viewing components

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <cprs.h>
#include <algorithm.h>
#include <QTimer>
#include <QTableWidget>

#include "include/graphics/canvas.h"
#include "include/models/constraint_model.h"
#include "include/window/port_dialog.h"
#include "include/network/drone_socket.h"
#include "include/network/ellipse_socket.h"
#include "include/network/waypoint_socket.h"
#include "include/network/point_socket.h"
#include "include/controls/compute_thread.h"

namespace optgui {

class Controller : public QObject {
    Q_OBJECT

 public:
    explicit Controller(Canvas *canvas);
    ~Controller();

    // model being manipulated
    ConstraintModel *model_;

    // SkyFly compute thread
    ComputeThread *compute_thread_;

    // add constraints
    void addEllipse(QPointF const &point, qreal radius = 120);
    void addPolygon(QVector<QPointF> points);
    void addPlane(QPointF const &p1, QPointF const &p2);

    // flip constraint direction
    void flipDirection(QGraphicsItem *item);

    // functions to add points for vehicle, obstacle, and waypoint locations
    void addWaypoint(QPointF const &point);
    void addPathPoint(QPointF const &point);
    void addFinalPoint(QPointF const &pos);
    void clearPathPoints();
    void removeAllWaypoints();
    void removeItem(QGraphicsItem *item);
    void duplicateSelected();

    // Set skyenet params
    void setSkyeFlyParams(QTableWidget *params_table);
    void setFinaltime(qreal final_time);

    // network functionality
    void setPorts();

    // control for executing traj
    void execute();
    void stageTraj();
    void unstageTraj();

    // toggle simulate traj
    void setSimulated(bool state);

 signals:
    // send traj to drone
    void trajectoryExecuted(autogen::packet::traj3dof data);
    // start/kill compute thread
    void startComputeWorker();
    void stopComputeWorker();

 private slots:
    // bind network sockets
    void startSockets();
    // set traj color
    void setPathColor(bool isRed);
    // update simulated traj
    void tickLiveReference();

 private:
    // QGraphicsScene canvas
    Canvas *canvas_;

    // render levels for ordering
    qreal final_point_render_level_;
    qreal waypoints_render_level_;

    // flag for simulated traj
    bool is_simulated_;

    // freeze traj timer
    QTimer *freeze_traj_timer_;
    quint32 traj_index_;

    // network configuration dialog box
    PortDialog *port_dialog_;

    // network sockets
    DroneSocket *drone_socket_;
    QVector<PointSocket *> final_point_sockets_;
    QVector<WaypointSocket *> waypoint_sockets_;
    QVector<EllipseSocket *> ellipse_sockets_;

    // remove and close sockets
    void removeEllipseSocket(EllipseModelItem *model);
    void removePointSocket(PointModelItem *model);
    void removeWaypointSocket(PointModelItem *model);
    void closeSockets();

    // load a graphical object from the model data
    void loadPoint(PointModelItem *model);
    void loadEllipse(EllipseModelItem *model);
    void loadPolygon(PolygonModelItem *model);
    void loadPlane(PlaneModelItem *model);
    void loadWaypoint(PointModelItem *model);

    // controls for staging/unstaging traj
    void freeze_traj();
    void setStagedPath();
    void unsetStagedPath();
};

}  // namespace optgui

#endif  // CONTROLLER_H_
