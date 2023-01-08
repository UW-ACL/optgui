// TITLE:   Optimization_Interface/src/controls/controller.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/controls/controller.h"

#include <QMessageBox>
#include <QSettings>
#include <QTranslator>
#include <QSet>
#include <QDate>
#include <QTextStream>
#include <QString>

#include <cmath>
#include <limits>

#include "include/graphics/point_graphics_item.h"
#include "include/graphics/ellipse_graphics_item.h"
#include "include/graphics/polygon_graphics_item.h"
#include "include/graphics/plane_graphics_item.h"
#include "include/graphics/polygon_resize_handle.h"
#include "include/graphics/path_graphics_item.h"
#include "include/graphics/drone_graphics_item.h"
#include "include/graphics/waypoint_graphics_item.h"
#include "include/globals.h"

namespace optgui {

Controller::Controller(Canvas *canvas) {
    // set canvas and create new model
    this->canvas_ = canvas;
    this->model_ = new ConstraintModel();

    this->loaded_model_ = new ConstraintModel();

    // set rendering order
    qreal renderLevel = std::numeric_limits<qreal>::max();

    // initialize drone graphic render level
    this->drone_render_level_ = renderLevel;
    renderLevel = std::nextafter(renderLevel, 0);

    // initialize final point graphic render level
    this->final_point_render_level_ = renderLevel;
    renderLevel = std::nextafter(renderLevel, 0);

    // initialize waypoint graphics render level
    this->waypoints_render_level_ = renderLevel;
    renderLevel = std::nextafter(renderLevel, 0);

    // initialize trajectory sent model and graphic
    PathModelItem *trajectory_sent_model = new PathModelItem();
    this->model_->setPathStagedModel(trajectory_sent_model);
    this->canvas_->path_staged_graphic_ =
            new PathGraphicsItem(trajectory_sent_model);
    this->canvas_->path_staged_graphic_->setColor(GREEN);
    this->canvas_->path_staged_graphic_->setZValue(renderLevel);
    renderLevel = std::nextafter(renderLevel, 0);
    this->canvas_->addItem(this->canvas_->path_staged_graphic_);

    // initialize trajectory model and graphic
    this->traj_render_level_ = renderLevel;
    renderLevel = std::nextafter(renderLevel, 0);

    // initialize port dialog
    this->port_dialog_ = new PortDialog();
    connect(this->port_dialog_, SIGNAL(setSocketPorts()),
            this, SLOT(startSockets()));

    // initialize save dialog
    this->save_dialog_ = new SaveDialog();

    // initialize load dialog
    this->load_dialog_ = new LoadDialog();

    // Initialize freeze_traj timer
    this->freeze_traj_timer_ = new QTimer();
    connect(this->freeze_traj_timer_, SIGNAL(timeout()),
            this, SLOT(tickLiveReference()));
    this->is_simulated_ = false;

    // Set traj lock. Cannot execute traj while already executing.
    this->traj_lock_ = false;

    // capture data on by default
    this->capture_data_ = true;
    this->output_file_ = nullptr;
}

Controller::~Controller() {
    for (ComputeThread *thread : this->compute_threads_) {
        thread->stopCompute();
        // delete should be handled by deletelater slot
//        thread->quit();
//        thread->wait();
//        delete thread;
    }
    this->compute_threads_.clear();

    // deinitialize port dialog
    delete this->port_dialog_;

    // deinitialize save dialog
    delete this->save_dialog_;

    // deinitialize network
    this->closeSockets();

    // clean up model
    delete this->model_;

    // clean up timer
    delete this->freeze_traj_timer_;
}

// ============ MENU CONTROLS ============

void Controller::setSkyeFlyParams(QTableWidget *params_table) {
    this->model_->setSkyeFlyParams(params_table);
}

void Controller::setFinaltime(qreal final_time) {
    this->model_->setFinaltime(final_time);
}

// ============ MOUSE CONTROLS ============

void Controller::removeItem(QGraphicsItem *item) {
    // switch based on custom graphics type
    switch (item->type()) {
        case DRONE_GRAPHIC: {
            // get drone model
            DroneGraphicsItem *drone = qgraphicsitem_cast<
                    DroneGraphicsItem *>(item);
            DroneModelItem *model = drone->model_;

            // remove drone socket
            this->removeDroneSocket(model);

            // stop staged or executed drones
            this->freeze_traj_timer_->stop();
            this->model_->setLiveReferenceMode(false);
            this->unsetStagedPath();

            // remove compute thread
            QMap<DroneModelItem *, ComputeThread *>::iterator iter =
                    this->compute_threads_.find(model);
            if (iter != this->compute_threads_.end()) {
                // get traj
                PathGraphicsItem *traj = (*iter)->getTrajGraphic();
                PathModelItem *traj_model = traj->model_;

                // stop compute
                (*iter)->stopCompute();
                // deletion will be handled by deletelater slot
                // remove from map
                iter = this->compute_threads_.erase(iter);

                // remove traj
                this->canvas_->removeItem(traj);
                this->canvas_->path_graphics_.remove(traj);
                delete traj;
                delete traj_model;
            }

            // remove drone
            if (this->model_->isCurrDrone(model)) {
                this->setCurrDrone(nullptr);
            }
            this->canvas_->removeItem(drone);
            this->canvas_->drone_graphics_.remove(drone);
            delete drone;
            this->model_->removeDrone(model);
            delete model;

            break;
        }
        case POINT_GRAPHIC: {
            // cast to specific graphic item type
            PointGraphicsItem *point = qgraphicsitem_cast<
                    PointGraphicsItem *>(item);
            // get data model
            PointModelItem *model = point->model_;
            for (ComputeThread *thread : this->compute_threads_.values()) {
                if (model == thread->getTarget()) {
                    thread->setTarget(nullptr);
                }
            }
            // delete network socket
            this->removePointSocket(model);
            // remove from QGraphicsScene canvas
            this->canvas_->removeItem(point);
            this->canvas_->final_points_.remove(point);
            // delete graphic
            delete point;
            // delete data model
            this->model_->removePoint(model);
            delete model;
            // exit switch
            break;
        }
        case ELLIPSE_GRAPHIC: {
            // cast to specific graphic item type
            EllipseGraphicsItem *ellipse = qgraphicsitem_cast<
                    EllipseGraphicsItem *>(item);
            // get data model
            EllipseModelItem *model = ellipse->model_;
            // delete network socket
            this->removeEllipseSocket(model);
            // remove from QGraphicsScene canvas
            this->canvas_->removeItem(ellipse);
            this->canvas_->ellipse_graphics_.removeOne(ellipse);
            // delete graphic
            delete ellipse;
            // delete data model
            this->model_->removeEllipse(model);
            delete model;
            // set new ordering of waypoints
            for (int i = 0; i < this->canvas_->ellipse_graphics_.size(); i++) {
                this->canvas_->ellipse_graphics_.at(i)->setIndex(i);
            }

            // exit switch
            break;
        }
        case POLYGON_GRAPHIC: {
            // cast to specific graphic item type
            PolygonGraphicsItem *polygon = qgraphicsitem_cast<
                    PolygonGraphicsItem *>(item);
            // get data model
            PolygonModelItem *model = polygon->model_;
            // remove from QGraphicsScene canvas
            this->canvas_->removeItem(polygon);
            this->canvas_->polygon_graphics_.remove(polygon);
            // delete graphic
            delete polygon;
            // delete data model
            this->model_->removePolygon(model);
            delete model;
            // exit switch
            break;
        }
        case PLANE_GRAPHIC: {
            // cast to specific graphic item type
            PlaneGraphicsItem *plane = qgraphicsitem_cast<
                    PlaneGraphicsItem *>(item);
            // get data model
            PlaneModelItem *model = plane->model_;
            // remove from QGraphicsScene canvas
            this->canvas_->removeItem(plane);
            this->canvas_->plane_graphics_.remove(plane);
            // delete graphic
            delete plane;
            // delete data model
            this->model_->removePlane(model);
            delete model;
            // exit switch
            break;
        }
        case WAYPOINT_GRAPHIC: {
            // cast to specific graphic item type
            WaypointGraphicsItem *waypoint =
                    dynamic_cast<WaypointGraphicsItem *>(item);
            // get data model
            PointModelItem *model = waypoint->model_;
            // delete network socket
            this->removePointSocket(model);
            // remove from QGraphicsScene canvas
            this->canvas_->removeItem(waypoint);
            this->canvas_->waypoint_graphics_.removeOne(waypoint);
            // delete graphic
            delete waypoint;
            // delete data model
            this->model_->removeWaypoint(model);
            delete model;
            // set new ordering of waypoints
            for (int i = 0; i < this->canvas_->waypoint_graphics_.size(); i++) {
                this->canvas_->waypoint_graphics_.at(i)->setIndex(i);
            }

            // update all traj's to no have waypoint
            for (ComputeThread *thread : this->compute_threads_) {
                thread->reInit();
            }

            break;
        }
    }
}

void Controller::flipDirection(QGraphicsItem *item) {
    // switch based on custom graphics type
    switch (item->type()) {
        case POLYGON_GRAPHIC: {
            // cast to specific graphic item type
            PolygonGraphicsItem *polygon = qgraphicsitem_cast<
                    PolygonGraphicsItem *>(item);
            // flip direction of keep out zone
            polygon->flipDirection();
            break;
        }
        case PLANE_GRAPHIC: {
            // cast to specific graphic item type
            PlaneGraphicsItem *plane = qgraphicsitem_cast<
                    PlaneGraphicsItem *>(item);
            // flip direction of keep out zone
            plane->flipDirection();
            break;
        }
        case WAYPOINT_GRAPHIC: {
            // reverse ordering of data models
            this->model_->reverseWaypoints();
            // reverse ordering of graphics items
            std::reverse(this->canvas_->waypoint_graphics_.begin(),
                         this->canvas_->waypoint_graphics_.end());
            // label with new ordering
            for (int i = 0; i < this->canvas_->waypoint_graphics_.size(); i++) {
                this->canvas_->waypoint_graphics_.at(i)->setIndex(i);
            }

            break;
        }
    }
}

void Controller::addEllipse(QPointF const &point, qreal radius) {
    // create new data model
    EllipseModelItem *item_model = new EllipseModelItem(point,
        this->model_->getClearance(), radius, radius, 0);
    // create graphic based on data model and save to model
    this->loadEllipse(item_model);
    // update color based on valid input code
    this->model_->updateEllipseColors();
}

void Controller::addPolygon(QVector<QPointF> points) {
    // create new data model
    PolygonModelItem *item_model = new PolygonModelItem(points);
    // create graphic based on data model and save to model
    this->loadPolygon(item_model);
}

void Controller::addPlane(QPointF const &p1, QPointF const &p2) {
    // create new data model
    PlaneModelItem *item_model = new PlaneModelItem(p1, p2);
    // create graphic based on data model and save to model
    this->loadPlane(item_model);
}

void Controller::addWaypoint(QPointF const &point) {
    // create new data model
    PointModelItem *item_model = new PointModelItem(point);
    // create graphic based on data model and save to model
    this->loadWaypoint(item_model);
}

void Controller::addFinalPoint(const QPointF &pos) {
    PointModelItem *item_model = new PointModelItem(pos);
    // create graphic based on data model and save to model
    this->loadPoint(item_model);
    this->setCurrFinalPoint(item_model);
}

void Controller::addDrone(const QPointF &pos) {
    // create drone model
    DroneModelItem *item_model = new DroneModelItem(pos);
    this->loadDrone(item_model);
    this->setCurrDrone(item_model);
}

void Controller::duplicateSelected() {
    for (QGraphicsItem *item : this->canvas_->selectedItems()) {
        // iterate over selected items and look for ellipse graphics items
        switch (item->type()) {
            case ELLIPSE_GRAPHIC: {
                // cast to ellipse graphic type
                EllipseGraphicsItem *ellipse = qgraphicsitem_cast<
                        EllipseGraphicsItem *>(item);
                // create new data model
                EllipseModelItem *new_model =
                        new EllipseModelItem(ellipse->model_->getPos(),
                                             ellipse->model_->getClearance(),
                                             ellipse->model_->getHeight(),
                                             ellipse->model_->getWidth(),
                                             ellipse->model_->getRot());
                // create graphic based on data model and save to model
                this->loadEllipse(new_model);
                break;
            }
        }
    }
}

// ============ BACK END CONTROLS ============

void Controller::setStagedDrone(DroneModelItem *drone_model) {
    for (DroneGraphicsItem *drone_graphic : this->canvas_->drone_graphics_) {
        if (drone_graphic->model_ == drone_model) {
            // select staged drone
            drone_graphic->is_staged_drone_ = true;
        } else {
            // deselect other staged drones
            drone_graphic->is_staged_drone_ = false;
        }
    }
}

void Controller::setExecutedDrone(DroneModelItem *drone_model) {
    for (DroneGraphicsItem *drone_graphic : this->canvas_->drone_graphics_) {
        if (drone_graphic->model_ == drone_model) {
            // select executed drone
            drone_graphic->is_executed_drone_ = true;
        } else {
            // deselect other executed drones
            drone_graphic->is_executed_drone_ = false;
        }
    }
}

void Controller::finalTime(DroneModelItem *drone, qreal time) {
    if (this->model_->isCurrDrone(drone)) {
        this->setFinaltime(time);
        emit this->finalTime(time);
    }
}

void Controller::updateMessage(DroneModelItem *drone) {
    if (this->model_->isCurrDrone(drone)) {
        emit this->updateMessage();
    }
}

void Controller::freeze_traj() {
    // compute time difference between each point on traj
    int msec = (1000 * this->model_->getFinaltime()) /
            (this->model_->getHorizon() - 1);
    // start timer to next time interval
    this->freeze_traj_timer_->start(msec);
    if (this->traj_lock_) {
        this->model_->setLiveReferenceMode(true);
    } else {
        this->model_->setLiveReferenceMode(false);
    }

    this->traj_index_ = 0;
    // update data output with traj
    if (this->capture_data_) {
        this->createOutputFile();
        this->updateOutputFile(this->model_->getStagedTraj3dof(),
                               this->model_->getStagedDrone(),
                               this->traj_index_);
    }
    this->traj_index_++;
}

void Controller::setStagedPath() {
    // stage the current trajectory
    this->model_->stageTraj();
    this->setStagedDrone(this->model_->getStagedDrone());
    this->canvas_->path_staged_graphic_->setColor(GREEN);
    // re-render staged traj
    this->canvas_->path_staged_graphic_->update(
                this->canvas_->path_staged_graphic_->boundingRect());
}

void Controller::unsetStagedPath() {
    // remove staged traj
    this->model_->unstageTraj();
    for (DroneGraphicsItem *drone : this->canvas_->drone_graphics_) {
        drone->is_staged_drone_ = false;
        drone->is_executed_drone_ = false;
    }
    this->canvas_->path_staged_graphic_->update(
                this->canvas_->path_staged_graphic_->boundingRect());
}

void Controller::tickLiveReference() {
    // move to next traj point in model
    if (this->model_->tickPathStaged()) {
        // more points in tracked traj
        autogen::packet::traj3dof traj = this->model_->getStagedTraj3dof();

        // get graphic for current drone
        DroneGraphicsItem *drone = nullptr;
        DroneModelItem *staged_drone = this->model_->getStagedDrone();
        for (DroneGraphicsItem *graphic : this->canvas_->drone_graphics_) {
            if (staged_drone == graphic->model_) {
                drone = graphic;
                break;
            }
        }

        int index = this->traj_index_;

        // update drone with reference telemetry in simulation mode
        if (this->is_simulated_) {
            QVector3D coords = nedToGuiXyz(traj.pos_ned(0, index),
                                           traj.pos_ned(1, index),
                                           traj.pos_ned(2, index));

            staged_drone->
                    setVel(nedToGuiXyz(traj.vel_ned(0, index),
                                       traj.vel_ned(1, index),
                                       traj.vel_ned(2, index)));
            staged_drone->
                    setAccel(nedToGuiXyz(traj.accl_ned(0, index),
                                         traj.accl_ned(1, index),
                                         traj.accl_ned(2, index)));

            // set model pos
            staged_drone->setPos(coords);
            // set graphic pos so view knows to draw offscreen
            drone->setPos(QPointF(coords.x(), coords.y()));
        }

        // update output file
        if (this->capture_data_ && this->output_file_ != nullptr) {
            this->updateOutputFile(traj, staged_drone, index);
        }

        this->traj_index_++;
    } else {
        // close output file
        if (this->output_file_ != nullptr) {
            this->output_file_->close();
            delete this->output_file_;
            this->output_file_ = nullptr;
        }

        // no more points in tracked traj
        // stop timer between traj time points
        this->freeze_traj_timer_->stop();
        // flag to stop tracking executed traj
        this->model_->setLiveReferenceMode(false);
        // unstage traj
        this->unsetStagedPath();
    }
    // re-render staged traj
    this->canvas_->path_staged_graphic_->update(
                this->canvas_->path_staged_graphic_->boundingRect());
}


void Controller::execute() {
    // get staged drone
    DroneModelItem *staged_drone = this->model_->getStagedDrone();

    if (!this->freeze_traj_timer_->isActive() &&
            this->model_->getIsTrajStaged()) {
        // start timer between traj
        this->freeze_traj();
        // set color to executed
        this->canvas_->path_staged_graphic_->setColor(CYAN);
        for (DroneGraphicsItem *drone : this->canvas_->drone_graphics_) {
            if (drone->model_ == staged_drone) {
                drone->is_executed_drone_ = true;
            } else {
                drone->is_executed_drone_ = false;
            }
        }

        // find traj and set it
        QMap<DroneModelItem *, ComputeThread *>::iterator iter =
                this->compute_threads_.find(staged_drone);
        if (iter != this->compute_threads_.end()) {
            (*iter)->getTrajGraphic()->model_->
                    setPoints(this->model_->getPathStagedPoints());
        }

        emit trajectoryExecuted(staged_drone,
                                this->model_->getStagedTraj3dof());
    } else if (this->freeze_traj_timer_->isActive() &&
               !this->traj_lock_ &&
               this->model_->getIsValidTraj() == FEASIBILITY_CODE::FEASIBLE) {
        this->setStagedPath();
        this->freeze_traj();
        this->canvas_->path_staged_graphic_->setColor(CYAN);
        // this->model_->setPathPoints(this->model_->getPathStagedPoints());

        emit trajectoryExecuted(staged_drone,
                                this->model_->getStagedTraj3dof());
    }
}

void Controller::createOutputFile() {
    // close old output file if exists
    if (this->output_file_ != nullptr) {
        this->output_file_->close();
        delete this->output_file_;
        this->output_file_ = nullptr;
    }

    // create file in same directory as executable
    QString filename = QDate::currentDate().toString("'data_'MM_dd_yyyy'");
    filename.append(QTime::currentTime().toString("'_'hh.mm.ss'.csv'"));

    this->output_file_ = new QFile(filename);

    if (this->output_file_->open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(this->output_file_);

        stream << "abs_time,rel_time,"

               << "pos_ref_n,pos_ref_e,pos_ref_d,"
               << "vel_ref_n,vel_ref_e,vel_ref_d,"
               << "accl_ref_n,accl_ref_e,accl_ref_d,"

               << "pos_telem_n,pos_telem_e,pos_telem_d,"
               << "vel_telem_n,vel_telem_e,vel_telem_d,"
               << "accl_telem_n,accl_telem_e,accl_telem_d,"

               << "pos_diff_n,pos_diff_e,pos_diff_d,"
               << "vel_diff_n,vel_diff_e,vel_diff_d,"
               << "accl_diff_n,accl_diff_e,accl_diff_d"

               << endl;
    } else {
        delete this->output_file_;
        this->output_file_ = nullptr;
    }
}

void Controller::updateOutputFile(autogen::packet::traj3dof const &traj, DroneModelItem *staged_drone, int index) {
    QTextStream stream(this->output_file_);

    // time
    stream << QTime::currentTime().toString("hh.mm.ss") << "," << traj.time(index) << ",";

    // ref
    stream << traj.pos_ned(0, index) << "," << traj.pos_ned(1, index) << "," << traj.pos_ned(2, index) << ","
           << traj.vel_ned(0, index) << "," << traj.vel_ned(1, index) << "," << traj.vel_ned(2, index) << ","
           << traj.accl_ned(0, index) << "," << traj.accl_ned(1, index) << "," << traj.accl_ned(2, index) << ",";

    // telem
    QVector3D pos_telem_ned(guiXyzToNED(staged_drone->getPos()));
    QVector3D vel_telem_ned(guiXyzToNED(staged_drone->getVel()));
    QVector3D accl_telem_ned(guiXyzToNED(staged_drone->getAccel()));
    stream << pos_telem_ned.x() << "," << pos_telem_ned.y() << "," << pos_telem_ned.z() << ","
           << vel_telem_ned.x() << "," << vel_telem_ned.y() << "," << vel_telem_ned.z() << ","
           << accl_telem_ned.x() << "," << accl_telem_ned.y() << "," << accl_telem_ned.z() << ",";

    // diff
    stream << traj.pos_ned(0, index)-pos_telem_ned.x() << "," << traj.pos_ned(1, index)-pos_telem_ned.y() << "," << traj.pos_ned(2, index)-pos_telem_ned.z() << ","
           << traj.vel_ned(0, index)-vel_telem_ned.x() << "," << traj.vel_ned(1, index)-vel_telem_ned.y() << "," << traj.vel_ned(2, index)-vel_telem_ned.z() << ","
           << traj.accl_ned(0, index)-accl_telem_ned.x() << "," << traj.accl_ned(1, index)-accl_telem_ned.y() << "," << traj.accl_ned(2, index)-accl_telem_ned.z();

    stream << endl;
}

void Controller::stageTraj() {
    // stage current traj if not currently tracking executed traj and
    // current traj is feasible
    if (!this->freeze_traj_timer_->isActive() &&
            this->model_->getIsValidTraj() == FEASIBILITY_CODE::FEASIBLE) {
        this->setStagedPath();
    }
}

void Controller::unstageTraj() {
    // unstage staged traj if not tracking executed traj
    if (!this->freeze_traj_timer_->isActive()) {
        this->unsetStagedPath();
    }
}

void Controller::setSimulated(bool state) {
    // flag to simulate traj instead of sending to vehicle
    this->is_simulated_ = state;
}

void Controller::setTrajLock(bool state) {
    this->traj_lock_ = state;
}

void Controller::setFreeFinalTime(bool state) {
    // TODO(dtsull16): reset inputs?
    this->model_->setFreeFinalTime(state);
}

void Controller::setDataCapture(bool state) {
    // close current output file when switching modes
    if (state != this->capture_data_ && this->output_file_ != nullptr) {
        this->output_file_->close();
        delete this->output_file_;
        this->output_file_ = nullptr;
    }
    // set state
    this->capture_data_ = state;
}

void Controller::setPorts() {
    // fill network config dialog with info from model
    this->port_dialog_->fillTable(this->model_);
    // open pop up dialog box
    this->port_dialog_->open();
}

void Controller::saveFile() {
    // save current configuration
    this->save_dialog_->saveConfig(this->model_);
}

void Controller::loadFile() {
    // load configuration from file
    this->load_dialog_->loadConfig(this->loaded_model_);

    ellipses_ = this->loaded_model_->getEllipses();
    waypoints_ = this->loaded_model_->getWaypoints();
    final_points_ = this->loaded_model_->getPoints();
    polygons_ = this->loaded_model_->getPolygons();
    drone_ = this->loaded_model_->getDrones();

    for (QVector<EllipseModelItem *>::iterator ptr = ellipses_.begin(); ptr != ellipses_.end(); ++ptr) {
        // create graphic based on data model and save to model
        this->loadEllipse(*ptr);
        // update color based on valid input code
        this->model_->updateEllipseColors();
    }
    for (QVector<PointModelItem *>::iterator ptr = waypoints_.begin(); ptr != waypoints_.end(); ++ptr) {
        // create graphic based on data model and save to model
        this->loadWaypoint(*ptr);
    }
    for (QSet<PointModelItem *>::iterator ptr = final_points_.begin(); ptr != final_points_.end(); ++ptr) {
        // create graphic based on data model and save to model
        this->loadPoint(*ptr);
    }
    for (QSet<PolygonModelItem *>::iterator ptr = polygons_.begin(); ptr != polygons_.end(); ++ptr) {
        // create graphic based on data model and save to model
        this->loadPolygon(*ptr);
    }
    if (drone_ != NULL){
        this->loadDrone(drone_);
    }

    this->loaded_model_ = new ConstraintModel();
    
}

// ============ NETWORK CONTROLS ============

void Controller::startSockets() {
    // close old sockets
    this->closeSockets();

    // create drone sockets
    for (DroneGraphicsItem *graphic : this->canvas_->drone_graphics_) {
        if (graphic->model_->port_ > 0) {
            DroneSocket *temp = new DroneSocket(graphic);
            connect(this,
                    SIGNAL(trajectoryExecuted(DroneModelItem *,
                                             const autogen::packet::traj3dof)),
                    temp,
                    SLOT(rx_trajectory(DroneModelItem *,
                                       const autogen::packet::traj3dof)));
            connect(temp, SIGNAL(refresh_graphics()),
                    this->canvas_, SLOT(update()));
            this->drone_sockets_.append(temp);
        }
    }

    // create final pos sockets
    for (PointGraphicsItem *graphic : this->canvas_->final_points_) {
        if (graphic->model_->port_ > 0) {
            PointSocket *temp = new PointSocket(graphic);
            connect(temp, SIGNAL(refresh_graphics()),
                    this->canvas_, SLOT(update()));
            this->final_point_sockets_.append(temp);
        }
    }

    // create waypoint sockets
    for (WaypointGraphicsItem *graphic : this->canvas_->waypoint_graphics_) {
        if (graphic->model_->port_ > 0) {
            WaypointSocket *temp = new WaypointSocket(graphic);
            connect(temp, SIGNAL(refresh_graphics()),
                    this->canvas_, SLOT(update()));
            this->waypoint_sockets_.append(temp);
        }
    }

    // create ellipse sockets
    for (EllipseGraphicsItem *graphic : this->canvas_->ellipse_graphics_) {
        if (graphic->model_->port_ > 0) {
            EllipseSocket *temp = new EllipseSocket(graphic);
            connect(temp, SIGNAL(refresh_graphics()),
                    this->canvas_, SLOT(update()));
            this->ellipse_sockets_.append(temp);
        }
    }
}

void Controller::closeSockets() {
    // close drone sockets
    for (DroneSocket *socket : this->drone_sockets_) {
        delete socket;
    }
    this->drone_sockets_.clear();

    // close final point sockets
    for (PointSocket *socket : this->final_point_sockets_) {
        delete socket;
    }
    this->final_point_sockets_.clear();

    // close waypoint sockets
    for (WaypointSocket *socket : this->waypoint_sockets_) {
        delete socket;
    }
    this->waypoint_sockets_.clear();

    // close ellipse sockets
    for (EllipseSocket *socket : this->ellipse_sockets_) {
        delete socket;
    }
    this->ellipse_sockets_.clear();
}

void Controller::removeEllipseSocket(EllipseModelItem *model) {
    // search for socket for given data model and delete it if
    // it exists
    int index = 0;
    bool found = false;

    for (EllipseSocket *socket : this->ellipse_sockets_) {
        if (socket->ellipse_item_->model_ == model) {
            delete socket;
            found = true;
            break;
        }
        index++;
    }

    if (found) {
        this->ellipse_sockets_.removeAt(index);
    }
}

void Controller::removePointSocket(PointModelItem *model) {
    // search for socket for given data model and delete it if
    // it exists
    int index = 0;
    bool found = false;

    for (PointSocket *socket : this->final_point_sockets_) {
        if (socket->point_item_->model_ == model) {
            delete socket;
            found = true;
            break;
        }
        index++;
    }

    if (found) {
        this->final_point_sockets_.removeAt(index);
    }
}

void Controller::removeDroneSocket(DroneModelItem *model) {
    int index = 0;
    bool found = false;

    for (DroneSocket *socket : this->drone_sockets_) {
        if (socket->drone_item_->model_ == model) {
            delete socket;
            found = true;
            break;
        }
        index++;
    }

    if (found) {
        this->drone_sockets_.removeAt(index);
    }
}

void Controller::removeWaypointSocket(PointModelItem *model) {
    // search for socket for given data model and delete it if
    // it exists
    int index = 0;
    bool found = false;

    for (WaypointSocket *socket : this->waypoint_sockets_) {
        if (socket->waypoint_item_->model_ == model) {
            delete socket;
            found = true;
            break;
        }
        index++;
    }

    if (found) {
        this->waypoint_sockets_.removeAt(index);
    }
}

// ============ LOAD CONTROLS ============

void Controller::loadEllipse(EllipseModelItem *item_model) {
    // create new graphic for data model
    quint32 index = this->canvas_->ellipse_graphics_.size();
    EllipseGraphicsItem *item_graphic =
            new EllipseGraphicsItem(item_model, index);
    // add graphic to canvas
    this->canvas_->addItem(item_graphic);
    this->canvas_->ellipse_graphics_.append(item_graphic);
    item_graphic->setRotation(item_model->getRot());
    // add graphic to model
    this->model_->addEllipse(item_model);
    // render graphic
    this->canvas_->bringToFront(item_graphic);
    item_graphic->update(item_graphic->boundingRect());
}

void Controller::loadPolygon(PolygonModelItem *item_model) {
    // create new graphic for data model
    PolygonGraphicsItem *item_graphic =
            new PolygonGraphicsItem(item_model);
    // add graphic to canvas
    this->canvas_->addItem(item_graphic);
    this->canvas_->polygon_graphics_.insert(item_graphic);
    // add graphic to model
    this->model_->addPolygon(item_model);
    // render graphic
    this->canvas_->bringToFront(item_graphic);
    item_graphic->update(item_graphic->boundingRect());
}

void Controller::loadPlane(PlaneModelItem *item_model) {
    // create new graphic for data model
    PlaneGraphicsItem *item_graphic =
            new PlaneGraphicsItem(item_model);
    // add graphic to canvas
    this->canvas_->addItem(item_graphic);
    this->canvas_->plane_graphics_.insert(item_graphic);
    // add graphic to model
    this->model_->addPlane(item_model);
    // render graphic
    this->canvas_->bringToFront(item_graphic);
    item_graphic->update(item_graphic->boundingRect());
}

void Controller::loadPoint(PointModelItem *item_model) {
    // create new graphic for data model
    PointGraphicsItem *item_graphic =
            new PointGraphicsItem(item_model);
    // add graphic to canvas
    this->canvas_->addItem(item_graphic);
    this->canvas_->final_points_.insert(item_graphic);
    // add graphic to model
    this->model_->addPoint(item_model);
    // render graphic
    item_graphic->setZValue(this->final_point_render_level_);
    item_graphic->update(item_graphic->boundingRect());
}

void Controller::loadDrone(DroneModelItem *item_model) {
    // create path model
    PathModelItem *trajectory_model = new PathModelItem();

    // create drone graphic
    DroneGraphicsItem *item_graphic =
            new DroneGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->canvas_->drone_graphics_.insert(item_graphic);
    this->model_->addDrone(item_model, trajectory_model);
    item_graphic->setZValue(this->drone_render_level_);
    item_graphic->update(item_graphic->boundingRect());

    // create path graphic
    PathGraphicsItem *path_graphic_ =
            new PathGraphicsItem(trajectory_model);
    path_graphic_->setZValue(this->traj_render_level_);
    this->canvas_->path_graphics_.insert(path_graphic_);
    this->canvas_->addItem(path_graphic_);

    // create compute thread
    ComputeThread *compute_thread_ =
            new ComputeThread(this->model_, item_graphic, path_graphic_);
    this->compute_threads_.insert(item_model, compute_thread_);
    connect(compute_thread_,
            SIGNAL(updateGraphics(PathGraphicsItem *, DroneGraphicsItem *)),
            this->canvas_,
            SLOT(updateGraphicsItems(PathGraphicsItem *, DroneGraphicsItem *)));
    connect(compute_thread_,
            SIGNAL(finalTime(DroneModelItem *, qreal)),
            this,
            SLOT(finalTime(DroneModelItem *, qreal)));
    connect(compute_thread_,
            SIGNAL(updateMessage(DroneModelItem *)),
            this,
            SLOT(updateMessage(DroneModelItem *)));
    connect(compute_thread_,
            SIGNAL(finished()),
            compute_thread_,
            SLOT(deleteLater()));
    compute_thread_->start();
}

void Controller::loadWaypoint(PointModelItem *item_model) {
    // create new graphic for data model
    quint32 index = this->canvas_->waypoint_graphics_.size();
    WaypointGraphicsItem *item_graphic =
            new WaypointGraphicsItem(item_model, index);
    // add graphic to canvas
    this->canvas_->addItem(item_graphic);
    this->canvas_->waypoint_graphics_.append(item_graphic);
    // add graphic to model
    this->model_->addWaypoint(item_model);
    // render graphic
    item_graphic->setZValue(this->waypoints_render_level_);
    item_graphic->update(item_graphic->boundingRect());

    // update all traj's with new waypoint
    for (ComputeThread *thread : this->compute_threads_) {
        thread->reInit();
    }
}

// ============ MODEL CONTROLS ============
quint32 Controller::getNumWaypoints() {
    return this->model_->getNumWaypoints();
}

void Controller::setClearance(qreal clearance) {
    this->model_->setClearance(clearance);
}

void Controller::setCurrFinalPoint(PointModelItem *point) {
    if (this->model_->getCurrDrone()) {
        QMap<DroneModelItem *, ComputeThread *>::iterator iter =
                this->compute_threads_.find(this->model_->getCurrDrone());
        if (iter != this->compute_threads_.end()) {
            (*iter)->setTarget(point);
        }
    }
}

void Controller::setCurrDrone(DroneModelItem *drone) {
    if (!this->model_->isCurrDrone(drone)) {
        this->model_->setCurrDrone(drone);

        if (drone != nullptr) {
            for (DroneGraphicsItem *drone_graphic :
                    this->canvas_->drone_graphics_) {
                if (drone_graphic->model_ == drone) {
                    drone_graphic->is_curr_drone_ = true;
                } else {
                    drone_graphic->is_curr_drone_ = false;
                }
            }
        }
    }
}

FEASIBILITY_CODE Controller::getIsValidTraj() {
    return this->model_->getIsValidTraj();
}

INPUT_CODE Controller::getIsValidInput() {
    return this->model_->getIsValidInput();
}

}  // namespace optgui
