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

    // SkyFly compute thread
    QMap<DroneModelItem *, ComputeThread *> compute_threads_;

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
    void addDrone(QPointF const &point);

    void clearPathPoints();
    void removeAllWaypoints();
    void removeItem(QGraphicsItem *item);
    void duplicateSelected();

    // Set skyenet params
    void setSkyeFlyParams(QTableWidget *params_table);
    void setFinaltime(qreal final_time);

    // network functionality
    void setPorts();

    void execute();
    void stageTraj();
    void setStagedDrone(DroneModelItem *drone);
    void setExecutedDrone(DroneModelItem *drone);
    void unstageTraj();

    void setSimulated(bool state);
    void setTrajLock(bool state);
    void setFreeFinalTime(bool state);

    // pass info between model and view
    quint32 getNumWaypoints();
    void setClearance(qreal clearance);
    void setCurrFinalPoint(PointModelItem *point);
    void setCurrDrone(DroneModelItem *drone);
    FEASIBILITY_CODE getIsValidTraj();
    INPUT_CODE getIsValidInput();

 signals:
    void trajectoryExecuted(DroneModelItem *, autogen::packet::traj3dof data);
    // signal view to update
    void finalTime(qreal time);
    void updateMessage();

 private slots:
    // receive update from compute thread, check if
    // drone is current drone
    void updateMessage(DroneModelItem *drone);
    void finalTime(DroneModelItem *drone, qreal time);
    void startSockets();
    void tickLiveReference();

 private:
    ConstraintModel *model_;

    // QGraphicsScene
    Canvas *canvas_;
    qreal drone_render_level_;
    qreal final_point_render_level_;
    qreal waypoints_render_level_;
    qreal traj_render_level_;

    bool is_simulated_;
    bool traj_lock_;

    // freeze traj timer
    QTimer *freeze_traj_timer_;
    quint32 traj_index_;

    // Port setting dialog and network sockets
    PortDialog *port_dialog_;
    QVector<DroneSocket *> drone_sockets_;
    QVector<PointSocket *> final_point_sockets_;
    QVector<WaypointSocket *> waypoint_sockets_;
    QVector<EllipseSocket *> ellipse_sockets_;

    void removeDroneSocket(DroneModelItem *model);
    void removeEllipseSocket(EllipseModelItem *model);
    void removePointSocket(PointModelItem *model);
    void removeWaypointSocket(PointModelItem *model);
    void closeSockets();

    void loadDrone(DroneModelItem *model);
    void loadPoint(PointModelItem *model);
    void loadEllipse(EllipseModelItem *model);
    void loadPolygon(PolygonModelItem *model);
    void loadPlane(PlaneModelItem *model);
    void loadWaypoint(PointModelItem *model);

    void freeze_traj();
    void setStagedPath();
    void unsetStagedPath();
};

}  // namespace optgui

#endif  // CONTROLLER_H_
