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

    // functions for target point data models
    // caller responsible for deleting pointer
    void addPoint(PointModelItem *item);
    void removePoint(PointModelItem *item);

    // functions for ellipse obstacle data models
    // caller responsible for deleting pointer
    void addEllipse(EllipseModelItem *item);
    void removeEllipse(EllipseModelItem *item);

    // functions for polygon constraint data models
    // caller responsible for deleting pointer
    void addPolygon(PolygonModelItem *item);
    void removePolygon(PolygonModelItem *item);

    // functions for plane constraint data models
    // caller responsible for deleting pointer
    void addPlane(PlaneModelItem *item);
    void removePlane(PlaneModelItem *item);

    // functions for waypoint data models
    // caller responsible for deleting pointer
    void addWaypoint(PointModelItem *item);
    void removeWaypoint(PointModelItem *item);
    quint32 getNumWaypoints();
    void reverseWaypoints();

    // functions for current traj data model
    void setPathModel(PathModelItem *model);
    void setPathPoints(QVector<QPointF> points);
    // get copy of trajectory points
    QVector<QPointF> getPathPoints();
    // clear traj
    void clearPathPoints();

    // functions for staged traj data model
    void setPathStagedModel(PathModelItem *model);
    void setPathStagedPoints(QVector<QPointF> points);
    // remove first point from staged traj, return whether
    // traj still has points
    bool tickPathStaged();
    // clear staged traj
    void clearPathStagedPoints();
    // get copy of staged traj points
    QVector<QPointF> getPathStagedPoints();

    // functions for vehicle data model
    void setDroneModel(DroneModelItem *model);
    void setDroneModelPos(QPointF const &pos);

    // functions for skyenet params
    skyenet::params getSkyeFlyParams();
    // copy params from expert panel to model params member
    void setSkyeFlyParams(QTableWidget *params_table);

    // functions for final time
    qreal getFinaltime();
    void setFinaltime(qreal finaltime);

    // functions for granularity of trajectory
    quint32 getHorizon();
    void setHorizon(quint32 horizon);

    // stage/unstage trajectory
    void stageTraj();
    void unstageTraj();

    // functions for current traj network packet
    autogen::packet::traj3dof getCurrTraj3dof();
    void setCurrTraj3dof(autogen::packet::traj3dof traj3dof_data);

    // functions for staged traj network packet
    autogen::packet::traj3dof getStagedTraj3dof();
    bool getIsTrajStaged();

    // functions for traj feasibility
    FEASIBILITY_CODE getIsValidTraj();
    void setIsValidTraj(FEASIBILITY_CODE code);

    // fill network configuration dialog table with
    // info from data models
    void fillTable(QTableWidget *port_table,
                   QTableWidget *drone_table,
                   QSet<quint16> *ports);

    // functions for clearance around ellipse obs (in meters)
    qreal getClearance();
    void setClearance(qreal clearance);

    // functions for whether gui is displaying a sent trajectory
    // (live reference)
    void setLiveReferenceMode(bool reference_mode);
    bool isLiveReference();

    // functions for valid input detection
    INPUT_CODE getIsValidInput();
    bool setIsValidInput(INPUT_CODE code);
    // calculate regions for ellipses to use for
    // overlap detection
    QVector<QRegion> getEllipseRegions();
    // mark overlapping ellipses as red
    void updateEllipseColors();

    // getters for inputs
    // coords in xyz, pos in pixels
    QPointF getFinalPos();
    QPointF getInitialPos();
    QPointF getInitialVel();
    QPointF getInitialAcc();
    QPointF getWpPos(int index);

    // functions for setting target point
    void setCurrFinalPoint(PointModelItem *point);
    bool hasCurrFinalPoint();
    bool isCurrFinalPoint(PointModelItem *model);

    // funtions for loading data into a skyenet params
    void loadWaypointConstraints(skyenet::params *P,
                                 double wp[3][skyenet::MAX_WAYPOINTS]);
    void loadEllipseConstraints(skyenet::params *P);
    void loadPosConstraints(skyenet::params *P);

private:
    // mutex lock for getters and setters
    QMutex model_lock_;

    // skyenet params
    skyenet::params P_;

    // trajectory network packets
    autogen::packet::traj3dof drone_curr_traj3dof_data_;
    autogen::packet::traj3dof drone_staged_traj3dof_data_;

    // input and feasibility status
    INPUT_CODE input_code_;
    FEASIBILITY_CODE feasible_code_;

    // flag for traj staged
    bool traj_staged_;
    // flag for tracking a sent trajectory
    bool is_live_reference_;

    // Clearance around ellipses in meters
    qreal clearance_;

    // Constraints
    QSet<EllipseModelItem *> ellipses_;
    QSet<PolygonModelItem *> polygons_;
    QSet<PlaneModelItem *> planes_;

    // waypoints
    QVector<PointModelItem *> waypoints_;

    // current trajectory
    PathModelItem *path_;
    // staged trajectory
    PathModelItem *path_staged_;
    // vehicle
    DroneModelItem *drone_;
    // target points
    QSet<PointModelItem *> final_points_;
    // currently selected target in final_points_
    PointModelItem *curr_final_point_;

    // Convert constraints to skyefly params
    void loadPlaneConstraint(skyenet::params *P, quint32 index,
                                 QPointF p, QPointF q);
    // helper function for distributing waypoints evenly among
    // the K points in the trajectory
    int distributeWpEvenly(skyenet::params *P, int index, int remaining,
                         int low, int high);
};

}  // namespace optgui

#endif  // CONSTRAINT_MODEL_H_
