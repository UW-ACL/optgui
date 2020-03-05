// TITLE:   Optimization_Interface/include/models/constraint_model.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Model holding data for all constraints and item info

#ifndef CONSTRAINT_MODEL_H_
#define CONSTRAINT_MODEL_H_

#include <QSet>
#include <QVector>
#include <QPointF>
#include <QMutex>
#include <QTableWidget>

#include "cprs.h"
#include "algorithm.h"
#include "autogen/lib.h"

#include "include/globals.h"
#include "include/models/point_model_item.h"
#include "include/models/ellipse_model_item.h"
#include "include/models/polygon_model_item.h"
#include "include/models/plane_model_item.h"
#include "include/models/path_model_item.h"
#include "include/models/drone_model_item.h"

namespace optgui {

class ConstraintModel {
 public:
    explicit ConstraintModel();

    ~ConstraintModel();

    void setFinalPointModel(PointModelItem *model);
    void setFinalPointPos(QPointF const &pos);

    void addEllipse(EllipseModelItem *item);
    void removeEllipse(EllipseModelItem *item);

    void addPolygon(PolygonModelItem *item);
    void removePolygon(PolygonModelItem *item);

    void addPlane(PlaneModelItem *item);
    void removePlane(PlaneModelItem *item);

    void setWaypointsModel(PathModelItem *model);
    void addWaypoint(QPointF const &pos);
    quint32 getNumWaypoints();

    void setPathModel(PathModelItem *model);
    void setPathPoints(QVector<QPointF> points);
    QVector<QPointF> getPathPoints();

    void setPathSentModel(PathModelItem *model);
    void setPathSentPoints(QVector<QPointF> points);
    void clearPathSentPoints();

    void setDroneModel(DroneModelItem *model);
    void setDroneModelPos(QPointF const &pos);

    void loadFinalPos(double r_f[3]);
    void loadInitialPos(double r_i[3]);
    void loadWaypoints(double wp[3][skyenet::MAX_WAYPOINTS]);

    skyenet::params getSkyeFlyParams();
    void setSkyeFlyParams(QTableWidget *params_table);

    qreal getFinaltime();
    void setFinaltime(qreal finaltime);

    quint32 getHorizon();
    void setHorizon(quint32 horizon);

    autogen::packet::traj3dof getTraj3dof();
    void setTraj3dof(autogen::packet::traj3dof traj3dof_data);

    bool getIsValidTraj();
    void setIsValidTraj(bool is_valid);

    void fillTable(QTableWidget *port_table,
                   QTableWidget *drone_table,
                   QSet<quint16> *ports);

 private:
    void initialize();

    QMutex model_lock_;

    // Skyefly params
    skyenet::params P_;
    autogen::packet::traj3dof drone_traj3dof_data_;
    bool is_valid_traj;

    // Constraints
    QSet<EllipseModelItem *> *ellipses_;
    QSet<PolygonModelItem *> *polygons_;
    QSet<PlaneModelItem *> *planes_;
    PathModelItem *waypoints_;
    PathModelItem *path_;
    PathModelItem *path_sent_;
    DroneModelItem *drone_;
    PointModelItem *final_pos_;

    // Convert constraints to skyefly params
    void loadEllipseConstraints(skyenet::params &P);
    void loadPosConstraints(skyenet::params &P);
    void loadPlaneConstraint(skyenet::params &P, quint32 index,
                                 QPointF p, QPointF q);

};

}  // namespace optgui

#endif  // CONSTRAINT_MODEL_H_
