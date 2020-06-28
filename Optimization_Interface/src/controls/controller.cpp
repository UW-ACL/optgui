// TITLE:   Optimization_Interface/src/controls/controller.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/controls/controller.h"

#include <QMessageBox>
#include <QSettings>
#include <QTranslator>
#include <QSet>

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

    // set rendering order
    qreal renderLevel = std::numeric_limits<qreal>::max();

    // initialize drone model and graphic
    DroneModelItem *drone_model = new DroneModelItem();
    this->model_->setDroneModel(drone_model);
    this->canvas_->drone_graphic_ =
            new DroneGraphicsItem(drone_model);
    this->canvas_->drone_graphic_->setZValue(renderLevel);
    renderLevel = std::nextafter(renderLevel, 0);
    this->canvas_->addItem(this->canvas_->drone_graphic_);

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
    PathModelItem *trajectory_model = new PathModelItem();
    this->model_->setPathModel(trajectory_model);
    this->canvas_->path_graphic_ =
            new PathGraphicsItem(trajectory_model);
    this->canvas_->path_graphic_->setZValue(renderLevel);
    renderLevel = std::nextafter(renderLevel, 0);
    this->canvas_->addItem(this->canvas_->path_graphic_);

    // initialize port dialog
    this->port_dialog_ = new PortDialog();
    connect(this->port_dialog_, SIGNAL(setSocketPorts()),
            this, SLOT(startSockets()));

    // Initialize network
    this->drone_socket_ = nullptr;

    // Initialize freeze_traj timer
    this->freeze_traj_timer_ = new QTimer();
    connect(this->freeze_traj_timer_, SIGNAL(timeout()),
            this, SLOT(tickLiveReference()));
    this->is_simulated_ = false;

    // initialize skyfly thread
    this->compute_thread_ = new ComputeThread(this->model_);
    connect(this->compute_thread_, SIGNAL(updateGraphics()), this->canvas_,
            SLOT(updatePathGraphicsItem()));
    connect(this->compute_thread_, SIGNAL(setPathColor(bool)),
            this, SLOT(setPathColor(bool)));
    connect(this, SIGNAL(stopComputeWorker()),
            this->compute_thread_, SLOT(stopCompute()));
    this->compute_thread_->start();
}

void Controller::setPathColor(bool isRed) {
    // set traj to red if compute tells us to
    if (isRed) {
        this->canvas_->path_graphic_->setColor(RED);
    } else {
        this->canvas_->path_graphic_->setColor(YELLOW);
    }
}

Controller::~Controller() {
    // signal to stop running the compute loop
    emit this->stopComputeWorker();
    // wait for current compute loop to finish then exit thread.
    // will error if thread is deleted before run() finishes
    this->compute_thread_->quit();
    this->compute_thread_->wait();
    delete this->compute_thread_;

    // deinitialize port dialog
    delete this->port_dialog_;

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
        case POINT_GRAPHIC: {
            // cast to specific graphic item type
            PointGraphicsItem *point = qgraphicsitem_cast<
                    PointGraphicsItem *>(item);
            // get data model
            PointModelItem *model = point->model_;
            // remove from curr final point
            if (this->model_->isCurrFinalPoint(model)) {
                this->model_->setCurrFinalPoint(nullptr);
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
            this->canvas_->ellipse_graphics_.remove(ellipse);
            // delete graphic
            delete ellipse;
            // delete data model
            this->model_->removeEllipse(model);
            delete model;
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

void Controller::freeze_traj() {
    // compute time difference between each point on traj
    int msec = (1000 * this->model_->getFinaltime()) /
            (this->model_->getHorizon() - 1);
    // start timer to next time interval
    this->freeze_traj_timer_->start(msec);
    // flag to start tracking sent traj
    this->model_->setLiveReferenceMode(true);
    // start tracking next index of network traj packet
    this->traj_index_ = 1;
}

void Controller::setStagedPath() {
    // stage the current trajectory
    this->model_->stageTraj();
    // set color of staged traj
    this->canvas_->path_staged_graphic_->setColor(GREEN);
    // re-render staged traj
    this->canvas_->path_staged_graphic_->update(
                this->canvas_->path_staged_graphic_->boundingRect());
}

void Controller::unsetStagedPath() {
    // remove staged traj
    this->model_->unstageTraj();
    // re-render staged traj
    this->canvas_->path_staged_graphic_->update(
                this->canvas_->path_staged_graphic_->boundingRect());
}

void Controller::tickLiveReference() {
    // move to next traj point in model
    if (this->model_->tickPathStaged()) {
        // more points in tracked traj
        autogen::packet::traj3dof traj = this->model_->getStagedTraj3dof();
        if (this->is_simulated_) {
            // get pos from staged traj
            QPointF coords = nedToGuiXyz(traj.pos_ned(0, this->traj_index_),
                                         traj.pos_ned(1, this->traj_index_));
            // set model pos
            this->canvas_->drone_graphic_->model_->setPos(coords);
            // set graphic pos so view knows to draw offscreen
            this->canvas_->drone_graphic_->setPos(coords);
        }
        // set velocity from traj
        this->canvas_->drone_graphic_->model_->
                setVel(nedToGuiXyz(traj.vel_ned(0, this->traj_index_),
                                   traj.vel_ned(1, this->traj_index_)));
        // set acceleration from traj
        this->canvas_->drone_graphic_->model_->
                setAccel(nedToGuiXyz(traj.accl_ned(0, this->traj_index_),
                                     traj.accl_ned(1, this->traj_index_)));
        // update to next traj index
        this->traj_index_++;
    } else {
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
    // do nothing if already executed or no staged traj
    if (!this->freeze_traj_timer_->isActive() &&
            this->model_->getIsTrajStaged()) {
        // start timer between traj
        this->freeze_traj();
        // set color to executed
        this->canvas_->path_staged_graphic_->setColor(CYAN);
        // set traj to executed traj
        this->model_->setPathPoints(this->model_->getPathStagedPoints());
        // signal to send traj to drone if not simulated
        if (!this->is_simulated_) {
            emit trajectoryExecuted(this->model_->getStagedTraj3dof());
        }
    }
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

void Controller::setPorts() {
    // fill network config dialog with info from model
    this->port_dialog_->fillTable(this->model_);
    // open pop up dialog box
    this->port_dialog_->open();
}

// ============ NETWORK CONTROLS ============

void Controller::startSockets() {
    // close old sockets
    this->closeSockets();

    // create drone socket
    if (this->canvas_->drone_graphic_->model_->port_ > 0) {
        this->drone_socket_ = new DroneSocket(
                    this->canvas_->drone_graphic_);
        connect(this,
                SIGNAL(trajectoryExecuted(const autogen::packet::traj3dof)),
                this->drone_socket_,
                SLOT(rx_trajectory(const autogen::packet::traj3dof)));
        connect(this->drone_socket_, SIGNAL(refresh_graphics()),
                this->canvas_, SLOT(update()));
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
                    this->canvas_, SLOT(updateEllipseGraphicsItem(graphic)));
            this->ellipse_sockets_.append(temp);
        }
    }
}

void Controller::closeSockets() {
    // delete drone socket if it exists
    if (this->drone_socket_) {
        delete this->drone_socket_;
        this->drone_socket_ = nullptr;
    }
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
    EllipseGraphicsItem *item_graphic =
            new EllipseGraphicsItem(item_model);
    // add graphic to canvas
    this->canvas_->addItem(item_graphic);
    this->canvas_->ellipse_graphics_.insert(item_graphic);
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
}

}  // namespace optgui
