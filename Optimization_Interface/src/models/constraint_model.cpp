// TITLE:   Optimization_Interface/src/models/constraint_model.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/models/constraint_model.h"

#include <QString>
#include <QLineF>

#include <algorithm>

#include "include/window/port_dialog/port_selector.h"
#include "include/window/port_dialog/drone_port_selector.h"
#include "include/window/port_dialog/drone_ip_selector.h"

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
    this->drone_ = nullptr;

    // initialize algorithm variables
    this->finaltime_ = 3;
    this->horizon_length_ = skyenet::MAX_HORIZON;
    this->is_valid_traj = false;
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

void ConstraintModel::setPathModel(PathModelItem *trajectory) {
    this->model_lock_.lock();
    if (this->path_) {
        delete this->path_;
    }
    this->path_ = trajectory;
    this->model_lock_.unlock();
}

void ConstraintModel::setPathPoints(QVector<QPointF> points) {
    this->model_lock_.lock();
    if (this->path_) {
        this->path_->setPoints(points);
    }
    this->model_lock_.unlock();
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


double* ConstraintModel::loadFinalPos(double *r_f) {
    this->model_lock_.lock();
    QPointF ned_coords = guiXyzToNED(this->final_pos_->getPos());
    r_f[1] = ned_coords.x();
    r_f[2] = ned_coords.y();
    this->model_lock_.unlock();
    return r_f;
}

double* ConstraintModel::loadInitialPos(double *r_i) {
    this->model_lock_.lock();
    QPointF ned_coords = guiXyzToNED(this->drone_->getPos());
    r_i[1] = ned_coords.x();
    r_i[2] = ned_coords.y();
    this->model_lock_.unlock();
    return r_i;
}

quint32 ConstraintModel::loadEllipseConstraints(
        double *R, double *c_e, double *c_n) {
    quint32 index = 0;
    this->model_lock_.lock();
    for (EllipseModelItem *ellipse : *this->ellipses_) {
        R[index] = ellipse->getRadius() / GRID_SIZE;
        QPointF ned_coords = guiXyzToNED(ellipse->getPos());
        // TODO(mceowen): c_e and c_n are backward in Skyefly
        c_e[index] = ned_coords.x();
        c_n[index] = ned_coords.y();
        index++;
        if (index >= skyenet::MAX_OBS) {
            this->model_lock_.unlock();
            return index;
        }
    }
    this->model_lock_.unlock();
    return index;
}

quint32 ConstraintModel::loadPosConstraints(double* A, double* b) {
    quint32 index = 0;
    this->model_lock_.lock();
    for (PolygonModelItem *polygon : *this->polygons_) {
        quint32 size = polygon->getSize();
        for (quint32 i = 1; i < size + 1; i++) {
            QPointF ned_p = guiXyzToNED(polygon->getPointAt(i - 1));
            QPointF ned_q = guiXyzToNED(polygon->getPointAt(i % size));
            if (polygon->getDirection()) {
                this->loadPlaneConstraint(A, b, index, ned_q, ned_p);
            } else {
                this->loadPlaneConstraint(A, b, index, ned_p, ned_q);
            }
            index++;
            if (index >= skyenet::MAX_CPOS) {
                this->model_lock_.unlock();
                return index;
            }
        }
    }

    for (PlaneModelItem *plane : *this->planes_) {
        QPointF ned_p = guiXyzToNED(plane->getP1());
        QPointF ned_q = guiXyzToNED(plane->getP2());

        if (plane->getDirection()) {
            this->loadPlaneConstraint(A, b, index, ned_q, ned_p);
        } else {
            this->loadPlaneConstraint(A, b, index, ned_p, ned_q);
        }
        index++;
        if (index >= skyenet::MAX_CPOS) {
            this->model_lock_.unlock();
            return index;
        }
    }
    this->model_lock_.unlock();
    return index;
}

qreal ConstraintModel::getFinaltime() {
    this->model_lock_.lock();
    qreal temp = this->finaltime_;
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setFinaltime(qreal finaltime) {
    this->model_lock_.lock();
    this->finaltime_ = finaltime;
    this->model_lock_.unlock();
}

quint32 ConstraintModel::getHorizon() {
    this->model_lock_.lock();
    quint32 temp = this->horizon_length_;
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setHorizon(quint32 horizon) {
    this->model_lock_.lock();
    this->horizon_length_ = std::min(horizon, skyenet::MAX_HORIZON);
    this->model_lock_.unlock();
}

autogen::packet::traj3dof ConstraintModel::getTraj3dof() {
    this->model_lock_.lock();
    autogen::packet::traj3dof temp = this->drone_traj3dof_data_;
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setTraj3dof(autogen::packet::traj3dof traj3dof_data) {
    this->model_lock_.lock();
    this->drone_traj3dof_data_ = traj3dof_data;
    this->model_lock_.unlock();
}

bool ConstraintModel::getIsValidTraj() {
    this->model_lock_.lock();
    bool temp = this->is_valid_traj;
    this->model_lock_.unlock();
    return temp;
}

void ConstraintModel::setIsValidTraj(bool is_valid) {
    this->model_lock_.lock();
    this->is_valid_traj = is_valid;
    this->model_lock_.unlock();
}

void ConstraintModel::fillTable(QTableWidget *port_table,
                                QTableWidget *drone_table,
                                QSet<quint16> *ports) {
    this->model_lock_.lock();

    // Configure port table
    port_table->setRowCount(3 + this->ellipses_->size() +
                                   this->polygons_->size() +
                                   this->planes_->size());

    // Set drone
    port_table->setItem(0, 0, new QTableWidgetItem("Drone"));
    port_table->item(0, 0)->setFlags(Qt::ItemIsEnabled);
    ports->insert(this->drone_->port_);
    port_table->setCellWidget(0, 1,
            new PortSelector(ports, this->drone_,
                             port_table));

    // Set final point
    port_table->setItem(1, 0, new QTableWidgetItem("Final Point"));
    port_table->item(1, 0)->setFlags(Qt::ItemIsEnabled);
    ports->insert(this->final_pos_->port_);
    port_table->setCellWidget(1, 1,
            new PortSelector(ports, this->final_pos_,
                             port_table));

    // Set waypoints
    port_table->setItem(2, 0, new QTableWidgetItem("Waypoints"));
    port_table->item(2, 0)->setFlags(Qt::ItemIsEnabled);
    ports->insert(this->waypoints_->port_);
    port_table->setCellWidget(2, 1,
            new PortSelector(ports, this->waypoints_,
                             port_table));

    quint16 row = 3;
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

    // Set drone table
    drone_table->setRowCount(2);

    // Set drone ip
    drone_table->setItem(0, 0,
                                new QTableWidgetItem("Drone Ip Addr"));
    drone_table->item(0, 0)->setFlags(Qt::ItemIsEnabled);
    drone_table->setCellWidget(0, 1,
            new DroneIpSelector(this->drone_, drone_table));

    // Set drone destination port
    drone_table->setItem(1, 0, new QTableWidgetItem("Drone Port"));
    drone_table->item(1, 0)->setFlags(Qt::ItemIsEnabled);
    drone_table->setCellWidget(1, 1,
            new DronePortSelector(this->drone_, drone_table));
    this->model_lock_.unlock();
}

// Private functions, should not lock

void ConstraintModel::loadPlaneConstraint(double *A, double *b, quint32 index,
                                              QPointF ned_p, QPointF ned_q) {
    qreal c = ((ned_p.y() * ned_q.x()) - (ned_p.x() * ned_q.y()));

    qreal a1 = (ned_p.y() - ned_q.y()) / c;
    qreal a2 = (ned_p.x() - ned_q.x()) / c * -1.0;

    QLineF line(ned_p, ned_q);
    QPointF normal = line.normalVector().p2();
    qreal flip = ((a1 * normal.x()) + (a2 * normal.y()) < 1) ? -1 : 1;

    A[2 * index] = flip * a1;
    A[(2 * index) + 1] = flip * a2;
    b[index] = flip;
}

}  // namespace optgui


