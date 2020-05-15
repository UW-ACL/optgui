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

ConstraintModel::ConstraintModel() : model_lock_() {
    this->initialize();
}

void ConstraintModel::initialize() {
    // Set model containers
    this->curr_final_point_ = nullptr;
    this->waypoints_ = nullptr;
    this->path_ = nullptr;
    this->path_staged_ = nullptr;
    this->drone_ = nullptr;

    // initialize algorithm variables
    this->P_ = skyenet::getDefaultP();
    // TODO(bchasnov): need default wp_idx and wprelax in params
    this->P_.wp_idx[0] = 10;
    this->P_.wprelax[0] = this->P_.K / 2;

    this->input_code_ = INPUT_CODE::VALID_INPUT;
    this->feasible_code_ = FEASIBILITY_CODE::INFEASIBLE;
    this->traj_staged_ = false;

    // initialize clearance around ellipse constriants
    // in meters
    this->clearance_ = INIT_CLEARANCE;

    // initialize live reference mode to disable updating
    // current trajectory
    this->is_live_reference_ = false;
}

ConstraintModel::~ConstraintModel() {
    QMutexLocker locker(&this->model_lock_);
    // Delete ellipses
    for (EllipseModelItem *ellipse : this->ellipses_) {
        delete ellipse;
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
    if (this->waypoints_) {
        delete this->waypoints_;
    }
    // Delete path
    if (this->path_) {
        delete this->path_;
    }
    // Delete path sent
    if (this->path_staged_) {
        delete this->path_staged_;
    }
    // Delete drone
    if (this->drone_) {
        delete this->drone_;
    }

    // Delete final point
    for (PointModelItem * model : this->final_points_) {
        delete model;
    }
    this->final_points_.clear();
}

// Public functions, need to lock

void ConstraintModel::addPoint(PointModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->final_points_.insert(item);
}

void ConstraintModel::removePoint(PointModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    if (item == this->curr_final_point_) {
        this->curr_final_point_ = nullptr;
    }
    this->final_points_.remove(item);
}

void ConstraintModel::addEllipse(EllipseModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->ellipses_.insert(item);
}

void ConstraintModel::removeEllipse(EllipseModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->ellipses_.remove(item);
}

void ConstraintModel::addPolygon(PolygonModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->polygons_.insert(item);
}

void ConstraintModel::removePolygon(PolygonModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->polygons_.remove(item);
}

void ConstraintModel::addPlane(PlaneModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->planes_.insert(item);
}

void ConstraintModel::removePlane(PlaneModelItem *item) {
    QMutexLocker locker(&this->model_lock_);
    this->planes_.remove(item);
}

void ConstraintModel::setWaypointsModel(PathModelItem *waypoints) {
    QMutexLocker locker(&this->model_lock_);
    if (this->waypoints_) {
        delete this->waypoints_;
    }
    this->waypoints_ = waypoints;
}

void ConstraintModel::addWaypoint(QPointF const &pos) {
    QMutexLocker locker(&this->model_lock_);
    if (this->waypoints_) {
        this->waypoints_->addPoint(pos);
    }
}

quint32 ConstraintModel::getNumWaypoints() {
    QMutexLocker locker(&this->model_lock_);
    quint32 temp = 0;
    if (this->waypoints_) {
        temp = this->waypoints_->getSize();
    }
    return temp;
}

void ConstraintModel::setPathModel(PathModelItem *trajectory) {
    QMutexLocker locker(&this->model_lock_);
    if (this->path_) {
        delete this->path_;
    }
    this->path_ = trajectory;
}


void ConstraintModel::setPathStagedModel(PathModelItem *trajectory) {
    QMutexLocker locker(&this->model_lock_);
    if (this->path_staged_) {
        delete this->path_staged_;
    }
    this->path_staged_ = trajectory;
}

void ConstraintModel::setPathPoints(QVector<QPointF> points) {
    QMutexLocker locker(&this->model_lock_);
    if (this->path_) {
        this->path_->setPoints(points);
    }
}

QVector<QPointF> ConstraintModel::getPathPoints() {
    QMutexLocker locker(&this->model_lock_);
    QVector<QPointF> temp;
    if (this->path_) {
        temp = this->path_->getPoints();
    }
    return temp;
}

void ConstraintModel::clearPathPoints() {
    QMutexLocker locker(&this->model_lock_);
    if (this->path_) {
        this->path_->clearPoints();
    }
}

void ConstraintModel::setPathStagedPoints(QVector<QPointF> points) {
    QMutexLocker locker(&this->model_lock_);
    if (this->path_staged_) {
        this->path_staged_->setPoints(points);
    }
}

bool ConstraintModel::tickPathStaged() {
    QMutexLocker locker(&this->model_lock_);
    if (this->path_staged_) {
        this->path_staged_->removePointAt(0);
    }
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

void ConstraintModel::setDroneModel(DroneModelItem *drone_model) {
    QMutexLocker locker(&this->model_lock_);
    if (this->drone_) {
        delete this->drone_;
    }
    this->drone_ = drone_model;
}

void ConstraintModel::setDroneModelPos(QPointF const &pos) {
    QMutexLocker locker(&this->model_lock_);
    if (this->drone_) {
        this->drone_->setPos(pos);
    }
}

QPointF ConstraintModel::getFinalPos() {
    QMutexLocker locker(&this->model_lock_);
    if (this->curr_final_point_ != nullptr) {
        return this->curr_final_point_->getPos();
    } else {
        return QPointF();
    }
}

QPointF ConstraintModel::getInitialPos() {
    QMutexLocker locker(&this->model_lock_);
    return this->drone_->getPos();
}

QPointF ConstraintModel::getInitialVel() {
    QMutexLocker locker(&this->model_lock_);
    return this->drone_->getVel();
}

QPointF ConstraintModel::getInitialAcc() {
    QMutexLocker locker(&this->model_lock_);
    return this->drone_->getAccel();
}

QPointF ConstraintModel::getWpPos() {
    QMutexLocker locker(&this->model_lock_);
    if (this->waypoints_->getSize() > 0) {
        return this->waypoints_->getPointAt(0);
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

autogen::packet::traj3dof ConstraintModel::getCurrTraj3dof() {
    QMutexLocker locker(&this->model_lock_);
    return this->drone_curr_traj3dof_data_;
}

void ConstraintModel::setCurrTraj3dof(autogen::packet::traj3dof traj3dof_data) {
    QMutexLocker locker(&this->model_lock_);
    this->drone_curr_traj3dof_data_ = traj3dof_data;
}

autogen::packet::traj3dof ConstraintModel::getStagedTraj3dof() {
    QMutexLocker locker(&this->model_lock_);
    return this->drone_staged_traj3dof_data_;
}

void ConstraintModel::setStagedTraj3dof(
        autogen::packet::traj3dof traj3dof_data) {
    QMutexLocker locker(&this->model_lock_);
    this->drone_staged_traj3dof_data_ = traj3dof_data;
}

bool ConstraintModel::getIsTrajStaged() {
    QMutexLocker locker(&this->model_lock_);
    return this->traj_staged_;
}

void ConstraintModel::setIsTrajStaged(bool is_staged) {
    QMutexLocker locker(&this->model_lock_);
    this->traj_staged_ = is_staged;
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

void ConstraintModel::setCurrFinalPoint(PointModelItem *point) {
    QMutexLocker locker(&this->model_lock_);
    this->curr_final_point_ = point;
}

bool ConstraintModel::hasCurrFinalPoint() {
    QMutexLocker locker(&this->model_lock_);
    return (this->curr_final_point_ != nullptr);
}

bool ConstraintModel::isCurrFinalPoint(PointModelItem *model) {
    QMutexLocker locker(&this->model_lock_);
    return (model == this->curr_final_point_);
}

void ConstraintModel::setSkyeFlyParams(QTableWidget *params_table) {
    QMutexLocker locker(&this->model_lock_);

    // load skyenet::params from  expert panel table
    uint32 row_index = 0;
    this->P_.K = qobject_cast<QSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
//    this->P_.dK = qobject_cast<QSpinBox *>
//            (params_table->cellWidget(row_index++, 0))->value();
//    this->P_.n_recalcs = qobject_cast<QSpinBox *>
//            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.a_min = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.a_max = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.theta_max = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.q_max = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.max_iter = qobject_cast<QSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.Delta_i = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.lambda = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.alpha = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.dL_tol = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.rho_0 = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.rho_1 = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.rho_2 = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.rirelax = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.rfrelax = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.wprelax[0] = qobject_cast<QDoubleSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
    this->P_.wp_idx[0] = qobject_cast<QSpinBox *>
            (params_table->cellWidget(row_index++, 0))->value();
}

skyenet::params ConstraintModel::getSkyeFlyParams() {
    QMutexLocker locker(&this->model_lock_);
    // Circle constraints | H(r - p) |^2 > R^2 where p is the center of the
    // circle and R is the radius (H some linear transform)
    this->loadEllipseConstraints(&this->P_);
    // Affine constraints Ax leq b
    this->loadPosConstraints(&this->P_);
    // time intervals
    this->P_.dt = (this->P_.tf / (this->P_.K - 1.0));
    skyenet::params P = this->P_;
    // set waypoint relax to 0 if no waypoints
    if (this->waypoints_->getSize() < 1) {
        P.wprelax[0] = 0;
    }
    return P;
}

void ConstraintModel::fillTable(QTableWidget *port_table,
                                QTableWidget *drone_table,
                                QSet<quint16> *ports) {
    QMutexLocker locker(&this->model_lock_);

    // Set drone table
    drone_table->setRowCount(1);

    // Set drone id
    drone_table->setItem(0, 0, new QTableWidgetItem("Drone ID"));
    drone_table->item(0, 0)->setFlags(Qt::ItemIsEnabled);
    ports->insert(this->drone_->port_);
    drone_table->setCellWidget(0, 1,
            new DroneIdSelector(this->drone_, drone_table, ports));


    // Configure port table
    quint16 row = 0;
    port_table->setRowCount(this->final_points_.size() +
                            this->ellipses_.size() +
                            this->polygons_.size() +
                            this->planes_.size());

    // Set final points
    quint16 count = 1;
    for (PointModelItem * model : this->final_points_) {
        port_table->setItem(row, 0,
                new QTableWidgetItem("Final Point " + QString::number(count)));
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

    // Set polygons
    count = 1;
    for (PolygonModelItem *model : this->polygons_) {
        port_table->setItem(row, 0,
                new QTableWidgetItem("Polygon " + QString::number(count)));
        port_table->item(row, 0)->setFlags(Qt::ItemIsEnabled);

        port_table->setCellWidget(row, 1,
                new PortSelector(ports, model, port_table));
        ports->insert(model->port_);

        row++;
        count++;
    }

    // Set planes
    count = 1;
    for (PlaneModelItem *model : this->planes_) {
        port_table->setItem(row, 0,
                new QTableWidgetItem("Plane " + QString::number(count)));
        port_table->item(row, 0)->setFlags(Qt::ItemIsEnabled);

        port_table->setCellWidget(row, 1,
                new PortSelector(ports, model, port_table));
        ports->insert(model->port_);

        row++;
        count++;
    }
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

// ====== Private functions, should not lock =======

void ConstraintModel::loadEllipseConstraints(skyenet::params *P) {
    quint32 index = 0;
    for (EllipseModelItem *ellipse : this->ellipses_) {
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

        QPointF ned_coords = guiXyzToNED(ellipse->getPos());
        P->obs.c_n[index] = ned_coords.x();
        P->obs.c_e[index] = ned_coords.y();
        index++;
        if (index >= skyenet::MAX_OBS) {
            P->obs.n = index;
            return;
        }
    }
    P->obs.n = index;
}

void ConstraintModel::loadPosConstraints(skyenet::params *P) {
    quint32 index = 0;
    for (PolygonModelItem *polygon : this->polygons_) {
        quint32 size = polygon->getSize();
        for (quint32 i = 1; i < size + 1; i++) {
            QPointF ned_p = guiXyzToNED(polygon->getPointAt(i - 1));
            QPointF ned_q = guiXyzToNED(polygon->getPointAt(i % size));
            if (polygon->getDirection()) {
                this->loadPlaneConstraint(P, index, ned_q, ned_p);
            } else {
                this->loadPlaneConstraint(P, index, ned_p, ned_q);
            }
            index++;
            if (index >= skyenet::MAX_CPOS) {
                P->cpos.n = index;
                return;
            }
        }
    }

    for (PlaneModelItem *plane : this->planes_) {
        QPointF ned_p = guiXyzToNED(plane->getP1());
        QPointF ned_q = guiXyzToNED(plane->getP2());

        if (plane->getDirection()) {
            this->loadPlaneConstraint(P, index, ned_q, ned_p);
        } else {
            this->loadPlaneConstraint(P, index, ned_p, ned_q);
        }
        index++;
        if (index >= skyenet::MAX_CPOS) {
            P->cpos.n = index;
            return;
        }
    }
    P->cpos.n = index;
}

void ConstraintModel::loadPlaneConstraint(skyenet::params *P, quint32 index,
                                              QPointF ned_p, QPointF ned_q) {
    qreal c = ((ned_p.y() * ned_q.x()) - (ned_p.x() * ned_q.y()));

    qreal a1 = (ned_p.y() - ned_q.y()) / c;
    qreal a2 = (ned_p.x() - ned_q.x()) / c * -1.0;

    QLineF line(ned_p, ned_q);
    QPointF normal = line.normalVector().p2();
    qreal flip = ((a1 * normal.x()) + (a2 * normal.y()) < 1) ? -1 : 1;

    P->cpos.A[2 * index] = flip * a1;
    P->cpos.A[(2 * index) + 1] = flip * a2;
    P->cpos.b[index] = flip;
}

}  // namespace optgui


