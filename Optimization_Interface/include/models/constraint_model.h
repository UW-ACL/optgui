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
    ConstraintModel();
    ~ConstraintModel();

    void addPoint(PointModelItem *item);
    void removePoint(PointModelItem *item);

    void addEllipse(EllipseModelItem *item);
    void removeEllipse(EllipseModelItem *item);

    void addPolygon(PolygonModelItem *item);
    void removePolygon(PolygonModelItem *item);

    void addPlane(PlaneModelItem *item);
    void removePlane(PlaneModelItem *item);

    void addWaypoint(PointModelItem *item);
    void removeWaypoint(PointModelItem *item);
    quint32 getNumWaypoints();
    void reverseWaypoints();

    void setPathModel(PathModelItem *model);
    void setPathPoints(QVector<QPointF> points);
    QVector<QPointF> getPathPoints();
    void clearPathPoints();

    void setPathStagedModel(PathModelItem *model);
    void setPathStagedPoints(QVector<QPointF> points);
    bool tickPathStaged();
    void clearPathStagedPoints();
    QVector<QPointF> getPathStagedPoints();

    void addDrone(DroneModelItem *item);
    void removeDrone(DroneModelItem *item);

    skyenet::params getSkyeFlyParams();
    void setSkyeFlyParams(QTableWidget *params_table);

    qreal getFinaltime();
    void setFinaltime(qreal finaltime);

    quint32 getHorizon();
    void setHorizon(quint32 horizon);

    void stageTraj();
    void unstageTraj();

    autogen::packet::traj3dof getCurrTraj3dof();
    void setCurrTraj3dof(autogen::packet::traj3dof traj3dof_data);

    autogen::packet::traj3dof getStagedTraj3dof();
    bool getIsTrajStaged();

    FEASIBILITY_CODE getIsValidTraj();
    void setIsValidTraj(FEASIBILITY_CODE code);

    void fillTable(QTableWidget *port_table,
                   QTableWidget *drone_table,
                   QSet<quint16> *ports);

    qreal getClearance();
    void setClearance(qreal clearance);

    void setLiveReferenceMode(bool reference_mode);
    bool isLiveReference();
    void setFreeFinalTime(bool free_final_time);
    bool isFreeFinalTime();

    INPUT_CODE getIsValidInput();
    bool setIsValidInput(INPUT_CODE code);
    QVector<QRegion> getEllipseRegions();
    void updateEllipseColors();

    QVector3D getFinalPos();
    QVector3D getInitialPos();
    QVector3D getInitialVel();
    QVector3D getInitialAcc();
    QPointF getWpPos(int index);

    void setCurrFinalPoint(PointModelItem *point);
    bool hasCurrFinalPoint();
    bool isCurrFinalPoint(PointModelItem *point);

    void setCurrDrone(DroneModelItem *drone);
    bool hasCurrDrone();
    bool isCurrDrone(DroneModelItem *drone);

    void loadWaypointConstraints(skyenet::params *P,
                                 double wp[skyenet::MAX_WAYPOINTS][3]);
    void loadEllipseConstraints(skyenet::params *P);
    void loadPosConstraints(skyenet::params *P);

private:
    QMutex model_lock_;

    // Skyefly params
    skyenet::params P_;
    autogen::packet::traj3dof drone_curr_traj3dof_data_;
    autogen::packet::traj3dof drone_staged_traj3dof_data_;
    INPUT_CODE input_code_;
    FEASIBILITY_CODE feasible_code_;
    bool traj_staged_;
    bool is_live_reference_;
    bool is_free_final_time_;

    // Clearance around ellipses in meters
    qreal clearance_;

    // Constraints
    QSet<EllipseModelItem *> ellipses_;
    QSet<PolygonModelItem *> polygons_;
    QSet<PlaneModelItem *> planes_;
    QVector<PointModelItem *> waypoints_;
    PathModelItem *path_;
    PathModelItem *path_staged_;
    QSet<DroneModelItem *> drones_;
    QSet<PointModelItem *> final_points_;
    PointModelItem *curr_final_point_;
    DroneModelItem *curr_drone_;

    // Convert constraints to skyefly params
    void loadPlaneConstraint(skyenet::params *P, quint32 index,
                                 QVector3D p, QVector3D q);
    int distributeWpEvenly(skyenet::params *P, int index, int remaining,
                         int low, int high);
};

}  // namespace optgui

#endif  // CONSTRAINT_MODEL_H_
