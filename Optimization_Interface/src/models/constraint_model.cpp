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
    this->ellipses_ = new QSet<EllipseModelItem *>();
    this->polygons_ = new QSet<PolygonModelItem *>();
    this->planes_ = new QSet<PlaneModelItem *>();
    this->final_pos_ = nullptr;
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
    this->model_lock_.lock();
    // Delete ellipses
    for (EllipseModelItem *ellipse : *this->ellipses_) {
        delete ellipse;
    }
    delete this->ellipses_;

    // Delete polygons
    for (PolygonModelItem *polygon : *this->polygons_) {
        delete polygon;
    }
    delete this->polygons_;

    // Delete planes
    for (PlaneModelItem *plane : *this->planes_) {
        delete plane;
    }
    delete this->planes_;

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
    if (this->final_pos_) {
        delete this->final_pos_;
    }

    this->model_lock_.unlock();
}

// Public functions, need to lock

void ConstraintModel::addEllipse(EllipseModelItem *item) {
    this->model_lock_.lock();
    this->ellipses_->insert(item);
    this->model_lock_.unlock();
}

void ConstraintModel::removeEllipse(EllipseModelItem *item) {
    this->model_lock_.lock();
    this->ellipses_->remove(item);
    this->model_lock_.unlock();
}

void ConstraintModel::addPolygon(PolygonModelItem *item) {
    this->model_lock_.lock();
    this->polygons_->insert(item);
    this->model_lock_.unlock();
}

void ConstraintModel::removePolygon(PolygonModelItem *item) {
    this->model_lock_.lock();
    this->polygons_->remove(item);
    this->model_lock_.unlock();
}

void ConstraintModel::addPlane(PlaneModelItem *item) {
    this->model_lock_.lock();
    this->planes_->insert(item);
    this->model_lock_.unlock();
}

void ConstraintModel::removePlane(PlaneModelItem *item) {
    this->model_lock_.lock();
    this->planes_->remove(item);
    this->model_lock_.unlock();
}

void ConstraintModel::setWaypointsModel(PathModelItem *waypoints) {
    this->model_lock_.lock();
    if (this->waypoints_) {
        delete this->waypoints_;
    }
    this->waypoints_ = waypoints;
    this->model_lock_.unlock();
}

void ConstraintModel::addWaypoint(QPointF const &pos) {
    this->model_lock_.lock();
    if (this->waypoints_) {
        this->waypoints_->addPoint(pos);
    }
    this->model_lock_.unlock();
}

quint32 ConstraintModel::getNumWaypoints() {
    this->model_lock_.lock();
    quint32 temp = 0;
    if (this->waypoints_) {
        temp = this->waypoints_->getSize();
    }
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setPathModel(PathModelItem *trajectory) {
    this->model_lock_.lock();
    if (this->path_) {
        delete this->path_;
    }
    this->path_ = trajectory;
    this->model_lock_.unlock();
}


void ConstraintModel::setPathStagedModel(PathModelItem *trajectory) {
    this->model_lock_.lock();
    if (this->path_staged_) {
        delete this->path_staged_;
    }
    this->path_staged_ = trajectory;
    this->model_lock_.unlock();
}

void ConstraintModel::setPathPoints(QVector<QPointF> points) {
    this->model_lock_.lock();
    if (this->path_) {
        this->path_->setPoints(points);
    }
    this->model_lock_.unlock();
}

QVector<QPointF> ConstraintModel::getPathPoints() {
    this->model_lock_.lock();
    QVector<QPointF> temp;
    if (this->path_) {
        temp = this->path_->getPoints();
    }
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::clearPathPoints() {
    this->model_lock_.lock();
    if (this->path_) {
        this->path_->clearPoints();
    }
    this->model_lock_.unlock();
}

void ConstraintModel::setPathStagedPoints(QVector<QPointF> points) {
    this->model_lock_.lock();
    if (this->path_staged_) {
        this->path_staged_->setPoints(points);
    }
    this->model_lock_.unlock();
}

bool ConstraintModel::tickPathStaged() {
    this->model_lock_.lock();
    if (this->path_staged_) {
        this->path_staged_->removePointAt(0);
    }
    this->model_lock_.unlock();
    return this->path_staged_->getSize() != 0;
}

void ConstraintModel::clearPathStagedPoints() {
    this->model_lock_.lock();
    if (this->path_staged_) {
        this->path_staged_->clearPoints();
    }
    this->model_lock_.unlock();
}

QVector<QPointF> ConstraintModel::getPathStagedPoints() {
    this->model_lock_.lock();
    QVector<QPointF> temp;
    if (this->path_staged_) {
        temp = this->path_staged_->getPoints();
    }
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setDroneModel(DroneModelItem *drone_model) {
    this->model_lock_.lock();
    if (this->drone_) {
        delete this->drone_;
    }
    this->drone_ = drone_model;
    this->model_lock_.unlock();
}

void ConstraintModel::setDroneModelPos(QPointF const &pos) {
    this->model_lock_.lock();
    if (this->drone_) {
        this->drone_->setPos(pos);
    }
    this->model_lock_.unlock();
}

void ConstraintModel::setFinalPointModel(PointModelItem *final_point) {
    this->model_lock_.lock();
    if (this->final_pos_) {
        delete this->final_pos_;
    }
    this->final_pos_ = final_point;
    this->model_lock_.unlock();
}

void ConstraintModel::setFinalPointPos(QPointF const &pos) {
    this->model_lock_.lock();
    if (this->final_pos_) {
        this->final_pos_->setPos(pos);
    }
    this->model_lock_.unlock();
}


QPointF ConstraintModel::getFinalPos() {
    QMutexLocker locker(&this->model_lock_);
    return this->final_pos_->getPos();
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
    this->model_lock_.lock();
    qreal temp = this->P_.tf;
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setFinaltime(qreal finaltime) {
    this->model_lock_.lock();
    this->P_.tf = finaltime;
    this->model_lock_.unlock();
}

autogen::packet::traj3dof ConstraintModel::getCurrTraj3dof() {
    this->model_lock_.lock();
    autogen::packet::traj3dof temp = this->drone_curr_traj3dof_data_;
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setCurrTraj3dof(autogen::packet::traj3dof traj3dof_data) {
    this->model_lock_.lock();
    this->drone_curr_traj3dof_data_ = traj3dof_data;
    this->model_lock_.unlock();
}

autogen::packet::traj3dof ConstraintModel::getStagedTraj3dof() {
    this->model_lock_.lock();
    autogen::packet::traj3dof temp = this->drone_staged_traj3dof_data_;
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setStagedTraj3dof(
        autogen::packet::traj3dof traj3dof_data) {
    this->model_lock_.lock();
    this->drone_staged_traj3dof_data_ = traj3dof_data;
    this->model_lock_.unlock();
}

bool ConstraintModel::getIsTrajStaged() {
    this->model_lock_.lock();
    bool temp = this->traj_staged_;
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setIsTrajStaged(bool is_staged) {
    this->model_lock_.lock();
    this->traj_staged_ = is_staged;
    this->model_lock_.unlock();
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
    for (EllipseModelItem *ellipse : *this->ellipses_) {
        ellipse->setClearance(clearance);
    }
    this->clearance_ = clearance;
}

quint32 ConstraintModel::getHorizon() {
    this->model_lock_.lock();
    quint32 temp = this->P_.K;
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setHorizon(quint32 horizon) {
    this->model_lock_.lock();
    this->P_.K = horizon;
    this->model_lock_.unlock();
}

bool ConstraintModel::isLiveReference() {
    this->model_lock_.lock();
    bool temp = this->is_live_reference_;
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setLiveReferenceMode(bool reference_mode) {
    this->model_lock_.lock();
    this->is_live_reference_ = reference_mode;
    this->model_lock_.unlock();
}

void ConstraintModel::setSkyeFlyParams(QTableWidget *params_table) {
    this->model_lock_.lock();

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

    this->model_lock_.unlock();
}

skyenet::params ConstraintModel::getSkyeFlyParams() {
    this->model_lock_.lock();
    // Circle constraints | H(r - p) |^2 > R^2 where p is the center of the
    // circle and R is the radius (H some linear transform)
    this->loadEllipseConstraints(this->P_);
    // Affine constraints Ax leq b
    this->loadPosConstraints(this->P_);
    // time intervals
    this->P_.dt = (this->P_.tf / (this->P_.K - 1.0));
    skyenet::params P = this->P_;
    // set waypoint relax to 0 if no waypoints
    if (this->waypoints_->getSize() < 1) {
        P.wprelax[0] = 0;
    }
    this->model_lock_.unlock();
    return P;
}

void ConstraintModel::fillTable(QTableWidget *port_table,
                                QTableWidget *drone_table,
                                QSet<quint16> *ports) {
    this->model_lock_.lock();

    // Set drone table
    drone_table->setRowCount(1);

    // Set drone id
    drone_table->setItem(0, 0, new QTableWidgetItem("Drone ID"));
    drone_table->item(0, 0)->setFlags(Qt::ItemIsEnabled);
    ports->insert(this->drone_->port_);
    drone_table->setCellWidget(0, 1,
            new DroneIdSelector(this->drone_, drone_table));


    // Configure port table
    quint16 row = 0;
    port_table->setRowCount(1 + this->ellipses_->size() +
                                   this->polygons_->size() +
                                   this->planes_->size());

    // Set final point
    port_table->setItem(row, 0, new QTableWidgetItem("Final Point"));
    port_table->item(row, 0)->setFlags(Qt::ItemIsEnabled);
    ports->insert(this->final_pos_->port_);
    port_table->setCellWidget(row, 1,
            new PortSelector(ports, this->final_pos_,
                             port_table));
    row++;

    // Set ellipses
    quint16 count = 1;
    for (EllipseModelItem *model : *this->ellipses_) {
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
    for (PolygonModelItem *model : *this->polygons_) {
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
    for (PlaneModelItem *model : *this->planes_) {
        port_table->setItem(row, 0,
                new QTableWidgetItem("Plane " + QString::number(count)));
        port_table->item(row, 0)->setFlags(Qt::ItemIsEnabled);

        port_table->setCellWidget(row, 1,
                new PortSelector(ports, model, port_table));
        ports->insert(model->port_);

        row++;
        count++;
    }

    this->model_lock_.unlock();
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
    for (EllipseModelItem *ellipse : *this->ellipses_) {
        regions.append(ellipse->getRegion());
    }
    return regions;
}

void ConstraintModel::updateEllipseColors() {
    QMutexLocker locker(&this->model_lock_);

    if (this->input_code_ == INPUT_CODE::VALID_INPUT) {
        // show ellipses as valid
        for (EllipseModelItem *ellipse : *this->ellipses_) {
            ellipse->setIsOverlap(false);
        }
    } else {
        // show ellipses as invalid
        for (EllipseModelItem *ellipse : *this->ellipses_) {
            ellipse->setIsOverlap(true);
        }
    }
}

// ====== Private functions, should not lock =======

void ConstraintModel::loadEllipseConstraints(skyenet::params &P) {
    quint32 index = 0;
    for (EllipseModelItem *ellipse : *this->ellipses_) {
        P.obs.R[index] = 1;
        qreal a = (ellipse->getHeight() / GRID_SIZE) + this->clearance_;
        qreal inv_a = 1.0 / a;
        qreal b = (ellipse->getWidth() / GRID_SIZE) + this->clearance_;
        qreal inv_b = 1.0 / b;
        qreal t = ellipse->getRot();
        qreal sin_t = qSin(qDegreesToRadians(t));
        qreal cos_t = qCos(qDegreesToRadians(t));
        qreal cos_t_2 = qPow(cos_t, 2);
        qreal sin_t_2 = qPow(sin_t, 2);

        P.obs.M0[0][index] = (inv_a * cos_t_2) + (inv_b * sin_t_2);
        P.obs.M0[1][index] = (inv_a * sin_t * cos_t) - (inv_b * sin_t * cos_t);
        P.obs.M1[0][index] = (inv_a * sin_t * cos_t) - (inv_b * sin_t * cos_t);
        P.obs.M1[1][index] = (inv_a * sin_t_2) + (inv_b * cos_t_2);

        QPointF ned_coords = guiXyzToNED(ellipse->getPos());
        P.obs.c_n[index] = ned_coords.x();
        P.obs.c_e[index] = ned_coords.y();
        index++;
        if (index >= skyenet::MAX_OBS) {
            P.obs.n = index;
            return;
        }
    }
    P.obs.n = index;
}

void ConstraintModel::loadPosConstraints(skyenet::params &P) {
    quint32 index = 0;
    for (PolygonModelItem *polygon : *this->polygons_) {
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
                P.cpos.n = index;
                return;
            }
        }
    }

    for (PlaneModelItem *plane : *this->planes_) {
        QPointF ned_p = guiXyzToNED(plane->getP1());
        QPointF ned_q = guiXyzToNED(plane->getP2());

        if (plane->getDirection()) {
            this->loadPlaneConstraint(P, index, ned_q, ned_p);
        } else {
            this->loadPlaneConstraint(P, index, ned_p, ned_q);
        }
        index++;
        if (index >= skyenet::MAX_CPOS) {
            P.cpos.n = index;
            return;
        }
    }
    P.cpos.n = index;
}

void ConstraintModel::loadPlaneConstraint(skyenet::params &P, quint32 index,
                                              QPointF ned_p, QPointF ned_q) {
    qreal c = ((ned_p.y() * ned_q.x()) - (ned_p.x() * ned_q.y()));

    qreal a1 = (ned_p.y() - ned_q.y()) / c;
    qreal a2 = (ned_p.x() - ned_q.x()) / c * -1.0;

    QLineF line(ned_p, ned_q);
    QPointF normal = line.normalVector().p2();
    qreal flip = ((a1 * normal.x()) + (a2 * normal.y()) < 1) ? -1 : 1;

    P.cpos.A[2 * index] = flip * a1;
    P.cpos.A[(2 * index) + 1] = flip * a2;
    P.cpos.b[index] = flip;
}

}  // namespace optgui


