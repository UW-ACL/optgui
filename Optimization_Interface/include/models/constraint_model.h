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

    void addPoint(PointModelItem *item);
    void removePoint(PointModelItem *item);

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
    void clearPathPoints();

    void setPathStagedModel(PathModelItem *model);
    void setPathStagedPoints(QVector<QPointF> points);
    bool tickPathStaged();
    void clearPathStagedPoints();
    QVector<QPointF> getPathStagedPoints();

    void setDroneModel(DroneModelItem *model);
    void setDroneModelPos(QPointF const &pos);

    skyenet::params getSkyeFlyParams();
    void setSkyeFlyParams(QTableWidget *params_table);

    qreal getFinaltime();
    void setFinaltime(qreal finaltime);

    quint32 getHorizon();
    void setHorizon(quint32 horizon);

    autogen::packet::traj3dof getCurrTraj3dof();
    void setCurrTraj3dof(autogen::packet::traj3dof traj3dof_data);

    autogen::packet::traj3dof getStagedTraj3dof();
    void setStagedTraj3dof(autogen::packet::traj3dof traj3dof_data);

    bool getIsTrajStaged();
    void setIsTrajStaged(bool is_staged);

    FEASIBILITY_CODE getIsValidTraj();
    void setIsValidTraj(FEASIBILITY_CODE code);

    void fillTable(QTableWidget *port_table,
                   QTableWidget *drone_table,
                   QSet<quint16> *ports);

    qreal getClearance();
    void setClearance(qreal clearance);

    void setLiveReferenceMode(bool reference_mode);
    bool isLiveReference();

    INPUT_CODE getIsValidInput();
    bool setIsValidInput(INPUT_CODE code);
    QVector<QRegion> getEllipseRegions();
    void updateEllipseColors();

    QPointF getFinalPos();
    QPointF getInitialPos();
    QPointF getInitialVel();
    QPointF getInitialAcc();
    QPointF getWpPos();

    void setCurrFinalPoint(PointModelItem *point);
    bool hasCurrFinalPoint();

 private:
    void initialize();

    QMutex model_lock_;

    // Skyefly params
    skyenet::params P_;
    autogen::packet::traj3dof drone_curr_traj3dof_data_;
    autogen::packet::traj3dof drone_staged_traj3dof_data_;
    INPUT_CODE input_code_;
    FEASIBILITY_CODE feasible_code_;
    bool traj_staged_;
    bool is_live_reference_;

    // Clearance around ellipses in meters
    qreal clearance_;

    // Constraints
    QSet<EllipseModelItem *> ellipses_;
    QSet<PolygonModelItem *> polygons_;
    QSet<PlaneModelItem *> planes_;
    PathModelItem *waypoints_;
    PathModelItem *path_;
    PathModelItem *path_staged_;
    DroneModelItem *drone_;
    QSet<PointModelItem *> final_points_;
    PointModelItem *curr_final_point_;

    // Convert constraints to skyefly params
    void loadEllipseConstraints(skyenet::params *P);
    void loadPosConstraints(skyenet::params *P);
    void loadPlaneConstraint(skyenet::params *P, quint32 index,
                                 QPointF p, QPointF q);

    bool is_frozen_;
};

}  // namespace optgui

#endif  // CONSTRAINT_MODEL_H_
