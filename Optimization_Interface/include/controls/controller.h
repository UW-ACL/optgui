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
#include "include/network/point_socket.h"
#include "include/controls/compute_thread.h"

namespace optgui {

class Controller : public QObject {
    Q_OBJECT

 public:
    explicit Controller(Canvas *canvas);
    ~Controller();

    void setCanvas(Canvas *canvas);  // sets up canvas for drawing graphics

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

    void execute();
    void stageTraj();
    void unstageTraj();

 signals:
    void trajectoryExecuted(autogen::packet::traj3dof data);
    void startComputeWorker();
    void stopComputeWorker();

 private slots:
    void startSockets();
    void setPathColor(bool isRed);
    void tickLiveReference();

 private:
    // QGraphicsScene
    Canvas *canvas_;
    qreal final_point_render_level_;

    // Freeze timer
    QTimer *freeze_timer_;
    quint32 traj_index_;

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

    void freeze();
    void setStagedPath();
    void unsetStagedPath();
};

}  // namespace optgui

#endif  // CONTROLLER_H_
