// TITLE:   Optimization_Interface/src/models/constraint_model.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/models/constraint_model.h"

#include <QString>
#include <QLineF>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QtMath>

#include <algorithm>

#include "include/window/port_dialog/port_selector.h"
#include "include/window/port_dialog/drone_id_selector.h"

namespace optgui {

ConstraintModel::ConstraintModel() : model_lock_(), P_() {
    // Set model containers
    this->curr_drone_ = nullptr;
    this->staged_drone_ = nullptr;
    this->path_staged_ = nullptr;

    this->input_code_ = INPUT_CODE::VALID_INPUT;
    this->feasible_code_ = FEASIBILITY_CODE::INFEASIBLE;
    this->traj_staged_ = false;

    // initialize clearance around ellipse constriants
    // in meters
    this->clearance_ = INIT_CLEARANCE;

    // initialize live reference mode to disable updating
    // current trajectory
    this->is_live_reference_ = false;
    this->is_free_final_time_ = false;
}

ConstraintModel::~ConstraintModel() {
    QMutexLocker locker(&this->model_lock_);
    // Delete ellipses
    for (EllipseModelItem *ellipse : this->ellipses_) {
        delete ellipse;
    }

    // Delete cylinders
    for (CylinderModelItem *cylinder : this->cylinders_) {
        delete cylinder;
    }

    // Delete polygons
    for (PolygonModelItem *polygon : this->polygons_) {
        delete polygon;
    }

    // Delete planes
    for (PlaneModelItem *plane : this->planes_) {
        delete plane;
    }

    // Delete waypoints
    for (PointModelItem *waypoint : this->waypoints_) {
        delete waypoint;
    }

    // Delete path staged
    if (this->path_staged_) {
        delete this->path_staged_;
    }
    // Delete drones and associated traj
    for ( QMap<DroneModelItem *, QPair<PathModelItem *,
          autogen::packet::traj2dof>>::iterator iter = this->drones_.begin();
         iter != this->drones_.end(); iter++) {
        delete iter.key();  // delete drone
        delete iter.value().first;  // delete path
    }

    // Delete final points
    for (PointModelItem *model : this->final_points_) {
        delete model;
    }
}

// Public functions, need to lock

void ConstraintModel::addPoint(PointModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->final_points_.insert(item);
}

void ConstraintModel::removePoint(PointModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->final_points_.remove(item);
}

QSet<PointModelItem *> ConstraintModel::getPoints() {
    return this->final_points_;
}

void ConstraintModel::addDrone(DroneModelItem *drone, PathModelItem *traj) {
    QMutexLocker locker(&this->model_lock_);
    this->drones_.insert(drone,
                         QPair<PathModelItem *, autogen::packet::traj2dof>
                                (traj, autogen::packet::traj2dof()));
}

void ConstraintModel::removeDrone(DroneModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    // unset curr drone
    if (item == this->curr_drone_) {
        this->curr_drone_ = nullptr;
    }
    this->drones_.remove(item);
}

DroneModelItem* ConstraintModel::getDrones(){
    if (drones_.count() > 0)
        return this->drones_.firstKey();
    else
        return NULL;
}

void ConstraintModel::addEllipse(EllipseModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->ellipses_.push_back(item);
}

void ConstraintModel::removeEllipse(EllipseModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    int idx = this->ellipses_.indexOf(item);
    this->ellipses_.remove(idx);
}

QVector<EllipseModelItem *> ConstraintModel::getEllipses(){
    return this->ellipses_;
}

void ConstraintModel::addCylinder(CylinderModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->cylinders_.push_back(item);
}

void ConstraintModel::removeCylinder(CylinderModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    int idx = this->cylinders_.indexOf(item);
    this->cylinders_.remove(idx);
}

QVector<CylinderModelItem *> ConstraintModel::getCylinders(){
    return this->cylinders_;
}

void ConstraintModel::addPolygon(PolygonModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->polygons_.insert(item);
}

void ConstraintModel::removePolygon(PolygonModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->polygons_.remove(item);
}

QSet<PolygonModelItem *> ConstraintModel::getPolygons() {
    return this->polygons_;
}

void ConstraintModel::addPlane(PlaneModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->planes_.insert(item);
}

void ConstraintModel::removePlane(PlaneModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->planes_.remove(item);
}

void ConstraintModel::addWaypoint(PointModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->waypoints_.append(item);
}

void ConstraintModel::removeWaypoint(PointModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->waypoints_.removeOne(item);
}

quint32 ConstraintModel::getNumWaypoints() {
    QMutexLocker locker(&this->model_lock_);
    return this->waypoints_.size();
}


void ConstraintModel::reverseWaypoints() {
    QMutexLocker locker(&this->model_lock_);
    std::reverse(this->waypoints_.begin(), this->waypoints_.end());
}

QVector<PointModelItem *> ConstraintModel::getWaypoints(){
    return this->waypoints_;
}

void ConstraintModel::setPathStagedModel(PathModelItem *trajectory) {
    QMutexLocker locker(&this->model_lock_);
    if (this->path_staged_) {
        delete this->path_staged_;
    }
    this->path_staged_ = trajectory;
}

void ConstraintModel::setPathStagedPoints(QVector<QPointF> points) {
    QMutexLocker locker(&this->model_lock_);
    if (this->path_staged_) {
        this->path_staged_->setPoints(points);
    }
}

bool ConstraintModel::tickPathStaged() {
    QMutexLocker locker(&this->model_lock_);
    // remove first point
    if (this->path_staged_) {
        this->path_staged_->removePointAt(0);
    }
    // return whether there are more points
    return this->path_staged_->getSize() != 0;
}

void ConstraintModel::clearPathStagedPoints() {
    QMutexLocker locker(&this->model_lock_);
    if (this->path_staged_) {
        this->path_staged_->clearPoints();
    }
}

QVector<QPointF> ConstraintModel::getPathStagedPoints() {
    QMutexLocker locker(&this->model_lock_);
    QVector<QPointF> temp;
    if (this->path_staged_) {
        temp = this->path_staged_->getPoints();
    }
    return temp;
}

QPointF ConstraintModel::getWpPos(int index) {
    QMutexLocker locker(&this->model_lock_);
    if (this->waypoints_.size() > index) {
        return this->waypoints_.at(index)->getPos();
    }
    return QPointF();
}

qreal ConstraintModel::getFinaltime() {
    QMutexLocker locker(&this->model_lock_);
    return this->P_.tf;
}

void ConstraintModel::setFinaltime(qreal finaltime) {
    QMutexLocker locker(&this->model_lock_);
    this->P_.tf = finaltime;
}

autogen::packet::traj2dof
        ConstraintModel::getCurrTraj2dof(DroneModelItem *drone) {
    QMutexLocker locker(&this->model_lock_);
    // find drone
    QMap<DroneModelItem *, QPair<PathModelItem *,
            autogen::packet::traj2dof>>::iterator iter =
            this->drones_.find(drone);
    if (iter != this->drones_.end()) {
        // get traj2dof for drone
        return (*iter).second;
    }
    return autogen::packet::traj2dof();
}

void ConstraintModel::setCurrTraj2dof(DroneModelItem *drone,
                                     autogen::packet::traj2dof traj2dof_data) {
    QMutexLocker locker(&this->model_lock_);
    // find drone
    QMap<DroneModelItem *, QPair<PathModelItem *,
            autogen::packet::traj2dof>>::iterator iter =
            this->drones_.find(drone);
    if (iter != this->drones_.end()) {
        // set traj2dof for drone
        (*iter).second = traj2dof_data;
    }
}

autogen::packet::traj2dof ConstraintModel::getStagedTraj2dof() {
    QMutexLocker locker(&this->model_lock_);
    return this->drone_staged_traj2dof_data_;
}

DroneModelItem *ConstraintModel::getStagedDrone() {
    QMutexLocker locker(&this->model_lock_);
    return this->staged_drone_;
}

void ConstraintModel::stageTraj() {
    QMutexLocker locker(&this->model_lock_);
    if (this->curr_drone_) {
        // set drone to staged drone
        this->staged_drone_ = this->curr_drone_;
        // find drone
        QMap<DroneModelItem *, QPair<PathModelItem *,
                autogen::packet::traj2dof>>::iterator iter =
                this->drones_.find(this->curr_drone_);
        if (iter != this->drones_.end()) {
            this->drone_staged_traj2dof_data_ = (*iter).second;
            this->path_staged_->setPoints((*iter).first->getPoints());
            this->traj_staged_ = true;
        }
    }
}

void ConstraintModel::unstageTraj() {
    QMutexLocker locker(&this->model_lock_);
    this->path_staged_->clearPoints();
    this->traj_staged_ = false;
}

bool ConstraintModel::getIsTrajStaged() {
    QMutexLocker locker(&this->model_lock_);
    return this->traj_staged_;
}

FEASIBILITY_CODE ConstraintModel::getIsValidTraj() {
    QMutexLocker locker(&this->model_lock_);
    return this->feasible_code_;
}

void ConstraintModel::setIsValidTraj(FEASIBILITY_CODE code) {
    QMutexLocker locker(&this->model_lock_);
    this->feasible_code_ = code;
}

void ConstraintModel::setClearance(qreal clearance) {
    QMutexLocker locker(&this->model_lock_);
    for (EllipseModelItem *ellipse : this->ellipses_) {
        ellipse->setClearance(clearance);
    }
    this->clearance_ = clearance;
}

quint32 ConstraintModel::getHorizon() {
    QMutexLocker locker(&this->model_lock_);
    return this->P_.K;
}

void ConstraintModel::setHorizon(quint32 horizon) {
    QMutexLocker locker(&this->model_lock_);
    this->P_.K = horizon;
}

bool ConstraintModel::isLiveReference() {
    QMutexLocker locker(&this->model_lock_);
    return this->is_live_reference_;
}

void ConstraintModel::setLiveReferenceMode(bool reference_mode) {
    QMutexLocker locker(&this->model_lock_);
    this->is_live_reference_ = reference_mode;
}

bool ConstraintModel::isFreeFinalTime() {
    QMutexLocker locker(&this->model_lock_);
    return this->is_free_final_time_;
}

void ConstraintModel::setFreeFinalTime(bool free_final_time) {
    QMutexLocker locker(&this->model_lock_);
    this->is_free_final_time_ = free_final_time;
}

void ConstraintModel::setCurrDrone(DroneModelItem *drone) {
    QMutexLocker locker(&this->model_lock_);
    this->curr_drone_ = drone;
}

bool ConstraintModel::hasCurrDrone() {
    QMutexLocker locker(&this->model_lock_);
    return (this->curr_drone_ != nullptr);
}

DroneModelItem *ConstraintModel::getCurrDrone() {
    QMutexLocker locker(&this->model_lock_);
    return this->curr_drone_;
}

bool ConstraintModel::isCurrDrone(DroneModelItem *drone) {
    QMutexLocker locker(&this->model_lock_);
    return (drone == this->curr_drone_);
}

void ConstraintModel::setSkyeFlyParams(QTableWidget *params_table) {
    QMutexLocker locker(&this->model_lock_);

    // load skyenet::params from  expert panel table
    uint32 row_index = 0;
    this->P_.K = qobject_cast<QSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    // no longer in expert panel table
//    this->P_.dK = qobject_cast<QSpinBox *>
//            (params_table->cellWidget(row_index++, 0))->value();
//    this->P_.n_recalcs = qobject_cast<QSpinBox *>
//            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.a_min = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.a_max = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.v_max = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.v_max_slow = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.theta_max = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.j_max = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.delta = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.max_iter = qobject_cast<QSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.lambda = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
//    this->P_.alpha = qobject_cast<QDoubleSpinBox *>
//            (params_table->cellWidget(row_index++, 0))->value();
//    this->P_.dL_tol = qobject_cast<QDoubleSpinBox *>
//            (params_table->cellWidget(row_index++, 0))->value();
//    this->P_.rho_0 = qobject_cast<QDoubleSpinBox *>
//            (params_table->cellWidget(row_index++, 0))->value();
//    this->P_.rho_1 = qobject_cast<QDoubleSpinBox *>
//            (params_table->cellWidget(row_index++, 0))->value();
//    this->P_.rho_2 = qobject_cast<QDoubleSpinBox *>
//            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.ri_relax = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.rf_relax = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.wp_relax = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.trust_tau_weight = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.trust_delta_weight = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
//    this->P_.wp_idx[0] = qobject_cast<QSpinBox *>
//            (params_table->cellWidget(row_index++, 0))->value();
}

skyenet::params ConstraintModel::getSkyeFlyParams() {
    QMutexLocker locker(&this->model_lock_);

    // time intervals
    // this->P_.dt = (this->P_.tf / (this->P_.K - 1.0));

    return this->P_;
}

void ConstraintModel::fillTable(QTableWidget *port_table,
                                QTableWidget *drone_table,
                                QSet<quint16> *ports) {
    QMutexLocker locker(&this->model_lock_);

    // Set drone table
    quint16 row = 0;
    drone_table->setRowCount(this->drones_.size());

    // Set drone ids
    quint16 count = 1;
    for (DroneModelItem *model : this->drones_.keys()) {
        drone_table->setItem(row, 0,
                new QTableWidgetItem("Vehicle "
                                     + QString::number(count)
                                     + " ID"));
        drone_table->item(row, 0)->setFlags(Qt::ItemIsEnabled);
        ports->insert(model->port_);
        drone_table->setCellWidget(row, 1,
                new DroneIdSelector(model, drone_table, ports));
        row++;
        count++;
    }

    // Configure port table
    row = 0;
    port_table->setRowCount(this->final_points_.size()
                            + this->waypoints_.size()
                            + this->ellipses_.size()
                            + this->cylinders_.size()
//                          + this->polygons_.size()
//                          + this->planes_.size()
                            );

    // Set final points
    count = 1;
    for (PointModelItem *model : this->final_points_) {
        port_table->setItem(row, 0,
                new QTableWidgetItem("Target " + QString::number(count)));
        port_table->item(row, 0)->setFlags(Qt::ItemIsEnabled);
        ports->insert(model->port_);
        port_table->setCellWidget(row, 1,
                new PortSelector(ports, model,
                                 port_table));
        row++;
        count++;
    }

    // Set waypoints
    count = 1;
    for (PointModelItem *model : this->waypoints_) {
        port_table->setItem(row, 0,
                new QTableWidgetItem("Waypoint " + QString::number(count)));
        port_table->item(row, 0)->setFlags(Qt::ItemIsEnabled);
        ports->insert(model->port_);
        port_table->setCellWidget(row, 1,
                new PortSelector(ports, model,
                                 port_table));
        row++;
        count++;
    }

    // Set ellipses
    count = 1;
    for (EllipseModelItem *model : this->ellipses_) {
        port_table->setItem(row, 0,
                new QTableWidgetItem("Ellipse " + QString::number(count)));
        port_table->item(row, 0)->setFlags(Qt::ItemIsEnabled);

        port_table->setCellWidget(row, 1,
                new PortSelector(ports, model, port_table));
        ports->insert(model->port_);

        row++;
        count++;
    }

    // Set cylinders
    count = 1;
    for (CylinderModelItem *model : this->cylinders_) {
        port_table->setItem(row, 0,
                new QTableWidgetItem("Cylinder " + QString::number(count)));
        port_table->item(row, 0)->setFlags(Qt::ItemIsEnabled);

        port_table->setCellWidget(row, 1,
                new PortSelector(ports, model, port_table));
        ports->insert(model->port_);

        row++;
        count++;
    }

    // Not Currently Supported
    // Would be interesting to implement for the purpose of centering
    //  a slow zone around a drone

    // Set polygons
//    count = 1;
//    for (PolygonModelItem *model : this->polygons_) {
//        port_table->setItem(row, 0,
//                new QTableWidgetItem("Polygon " + QString::number(count)));
//        port_table->item(row, 0)->setFlags(Qt::ItemIsEnabled);

//        port_table->setCellWidget(row, 1,
//                new PortSelector(ports, model, port_table));
//        ports->insert(model->port_);

//        row++;
//        count++;
//    }

    // Set planes
//    count = 1;
//    for (PlaneModelItem *model : this->planes_) {
//        port_table->setItem(row, 0,
//                new QTableWidgetItem("Plane " + QString::number(count)));
//        port_table->item(row, 0)->setFlags(Qt::ItemIsEnabled);

//        port_table->setCellWidget(row, 1,
//                new PortSelector(ports, model, port_table));
//        ports->insert(model->port_);

//        row++;
//        count++;
//    }
}

qreal ConstraintModel::getClearance() {
    QMutexLocker locker(&this->model_lock_);
    return this->clearance_;
}

INPUT_CODE ConstraintModel::getIsValidInput() {
    QMutexLocker locker(&this->model_lock_);
    return this->input_code_;
}

bool ConstraintModel::setIsValidInput(INPUT_CODE code) {
    QMutexLocker locker(&this->model_lock_);
    bool new_code = code != this->input_code_;
    this->input_code_ = code;
    return new_code;
}

QVector<QRegion> ConstraintModel::getEllipseRegions() {
    QMutexLocker locker(&this->model_lock_);
    QVector<QRegion> regions;
    for (EllipseModelItem *ellipse : this->ellipses_) {
        regions.append(ellipse->getRegion());
    }
    return regions;
}

void ConstraintModel::updateEllipseColors() {
    QMutexLocker locker(&this->model_lock_);

    if (this->input_code_ == INPUT_CODE::VALID_INPUT) {
        // show ellipses as valid
        for (EllipseModelItem *ellipse : this->ellipses_) {
            ellipse->setIsOverlap(false);
        }
    } else {
        // show ellipses as invalid
        for (EllipseModelItem *ellipse : this->ellipses_) {
            ellipse->setIsOverlap(true);
        }
    }
}

QVector<QRegion> ConstraintModel::getCylinderRegions() {
    QMutexLocker locker(&this->model_lock_);
    QVector<QRegion> regions;
    for (CylinderModelItem *cylinder : this->cylinders_) {
        regions.append(cylinder->getRegion());
    }
    return regions;
}

void ConstraintModel::updateCylinderColors() {
    QMutexLocker locker(&this->model_lock_);

    if (this->input_code_ == INPUT_CODE::VALID_INPUT) {
        // show cylinders as valid
        for (CylinderModelItem *cylinder : this->cylinders_) {
            cylinder->setIsOverlap(false);
        }
    } else {
        // show cylinders as invalid
        for (CylinderModelItem *cylinder : this->cylinders_) {
            cylinder->setIsOverlap(true);
        }
    }
}

void ConstraintModel::loadWaypointConstraints(
            skyenet::params *P,
            double wp[skyenet::MAX_WAYPOINTS][3]) {
    QMutexLocker locker(&this->model_lock_);

    P->n_wp = this->waypoints_.size();
    // no waypoints, dont factor in relaxation
    if (P->n_wp == 0) {
        P->wp_relax = 0;
        return;
    }

    // space out waypoint indicies
    this->distributeWpEvenly(P, 0, P->n_wp, 1, (P->K - 2));

    // load waypoint pos
    for (quint32 i = 0; i < P->n_wp; i++) {
        QPointF wp_pos = this->waypoints_.at(i)->getPos();
        QVector3D xyz_wp_pos = guiXyzToXyz(wp_pos.x(), wp_pos.y(), 0);
        wp[i][0] = xyz_wp_pos.x();
        wp[i][1] = xyz_wp_pos.y();
    }
}

void ConstraintModel::loadEllipseConstraints(skyenet::params *P) {
    QMutexLocker locker(&this->model_lock_);

    quint32 index = 0;
    for (EllipseModelItem *ellipse : this->ellipses_) {
        // calculate ellipse matrix in meters

        P->obs.R[index] = 1;
        qreal a = (ellipse->getHeight() / GRID_SIZE) + this->clearance_;
        qreal inv_a = 1.0 / a;
        qreal b = (ellipse->getWidth() / GRID_SIZE) + this->clearance_;
        qreal inv_b = 1.0 / b;
        qreal t = ellipse->getRot();
        qreal sin_t = qSin(qDegreesToRadians(t));
        qreal cos_t = qCos(qDegreesToRadians(t));
        qreal cos_t_2 = qPow(cos_t, 2);
        qreal sin_t_2 = qPow(sin_t, 2);

        P->obs.M0[0][index] = (inv_a * cos_t_2) + (inv_b * sin_t_2);
        P->obs.M0[1][index] = (inv_a * sin_t * cos_t) - (inv_b * sin_t * cos_t);
        P->obs.M1[0][index] = (inv_a * sin_t * cos_t) - (inv_b * sin_t * cos_t);
        P->obs.M1[1][index] = (inv_a * sin_t_2) + (inv_b * cos_t_2);

        QPointF ellipse_pos = ellipse->getPos();
        QVector3D xyz_coords = guiXyzToXyz(ellipse_pos.x(), ellipse_pos.y(), 0);
        P->obs.c_x[index] = xyz_coords.x();
        P->obs.c_y[index] = xyz_coords.y();
        index++;
        // dont go over max
        if (index >= skyenet::MAX_OBS) {
            P->obs.n = index;
            return;
        }
    }
    P->obs.n = index;
}

void ConstraintModel::loadCylinderConstraints(skyenet::params *P) {
    QMutexLocker locker(&this->model_lock_);

    quint32 index = 0;
    for (CylinderModelItem *cylinder : this->cylinders_) {
        // calculate cylinder properties

        qreal p_c = (cylinder->getWidth() / GRID_SIZE);
        qreal p_g = ((cylinder->getWidth() + cylinder->getTriggerWidth())/ GRID_SIZE);
        qreal l_c = (cylinder->getHeight() / GRID_SIZE);
        qreal yaw = cylinder->getRot();

        QPointF cylinder_pos = cylinder->getPos();
        QVector3D xyz_coords = guiXyzToXyz(cylinder_pos.x(), cylinder_pos.y(), 0);

        P->hoops.c_x[index] = xyz_coords.x();
        P->hoops.c_y[index] = xyz_coords.y();
        P->hoops.p_c[index] = p_c;
        P->hoops.p_g[index] = p_g;
        P->hoops.l_c[index] = l_c;
        P->hoops.yaw[index] = -yaw;
        index++;
        // dont go over max
        if (index >= skyenet::MAX_OBS) {
            P->hoops.n = index;
            return;
        }
    }
    P->hoops.n = index;
}

void ConstraintModel::loadPosConstraints(skyenet::params *P) {
    QMutexLocker locker(&this->model_lock_);

    quint32 index = 0;
    for (PolygonModelItem *polygon : this->polygons_) {
        quint32 size = polygon->getSize();
        for (quint32 i = 1; i < size + 1; i++) {
            QPointF p_pos = polygon->getPointAt(i - 1);
            QPointF q_pos = polygon->getPointAt(i % size);
            QVector3D xyz_p = guiXyzToXyz(p_pos.x(), p_pos.y(), 0);
            QVector3D xyz_q = guiXyzToXyz(q_pos.x(), q_pos.y(), 0);
            if (polygon->getDirection()) {
                this->loadPlaneConstraint(P, index, xyz_p, xyz_q);
            } else {
                this->loadPlaneConstraint(P, index, xyz_q, xyz_p);
            }
            index++;
            // dont go over max
            if (index >= skyenet::MAX_CPOS) {
                P->cpos.n = index;
                return;
            }
        }
    }

    for (PlaneModelItem *plane : this->planes_) {
        QPointF p1_pos = plane->getP1();
        QPointF p2_pos = plane->getP2();
        QVector3D xyz_p = guiXyzToXyz(p1_pos.x(), p1_pos.y(), 0);
        QVector3D xyz_q = guiXyzToXyz(p2_pos.x(), p2_pos.y(), 0);

        // choose direction of constraint
        if (plane->getDirection()) {
            this->loadPlaneConstraint(P, index, xyz_p, xyz_q);
        } else {
            this->loadPlaneConstraint(P, index, xyz_q, xyz_p);
        }
        index++;
        // dont go over max
        if (index >= skyenet::MAX_CPOS) {
            P->cpos.n = index;
            return;
        }
    }
    P->cpos.n = index;
}

// ====== Private functions, do not lock ======

void ConstraintModel::loadPlaneConstraint(skyenet::params *P, quint32 index,
                                          QVector3D xyz_p, QVector3D xyz_q) {
    qreal c = ((xyz_q.x() * xyz_p.y()) - (xyz_q.y() * xyz_p.x()));

    qreal a1 = (xyz_q.x() - xyz_p.x()) / c;
    qreal a2 = (xyz_q.y() - xyz_p.y()) / c * -1.0;

    QLineF line(QPointF(xyz_q.x(), xyz_q.y()),
                QPointF(xyz_p.x(), xyz_p.y()));
    QPointF normal = line.normalVector().p2();
    qreal flip = ((a1 * normal.y()) + (a2 * normal.x()) < 1) ? -1 : 1;

    P->cpos.A[2 * index] = flip * a1;
    P->cpos.A[(2 * index) + 1] = flip * a2;
    P->cpos.b[index] = flip;
}

int ConstraintModel::distributeWpEvenly(skyenet::params *P,
                                        int index, int remaining,
                                        int low, int high) {
    // recursively fill P->wp_index with evenly spaced indicies
    if (remaining != 0) {
        int mid = (low + high + 1) / 2;

        bool place = false;
        if (remaining % 2 != 0) {
            place = true;
            remaining--;
        }

        index = distributeWpEvenly(P, index, remaining / 2, low, mid - 1);

        if (place) {
            assert(P->wp_idx[index] == 0);
            P->wp_idx[index] = mid;
            index++;
        }

        index = distributeWpEvenly(P, index, remaining / 2, mid, high);
    }
    return index;
}

}  // namespace optgui


