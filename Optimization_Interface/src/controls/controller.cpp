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
#include "include/graphics/waypoints_graphics_item.h"
#include "include/graphics/waypoints_resize_handle.h"
#include "include/globals.h"

namespace optgui {

Controller::Controller(Canvas *canvas) {
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
    renderLevel = std::nextafter(renderLevel, 0);
    this->final_point_render_level_ = renderLevel;

    // initialize waypoints model and graphic
    PathModelItem *waypoint_model = new PathModelItem();
    this->model_->setWaypointsModel(waypoint_model);
    this->canvas_->waypoints_graphic_ =
            new WaypointsGraphicsItem(waypoint_model);
    this->canvas_->waypoints_graphic_->setZValue(renderLevel);
    renderLevel = std::nextafter(renderLevel, 0);
    this->canvas_->addItem(this->canvas_->waypoints_graphic_);

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

    // Initialize freeze timer
    this->freeze_timer_ = new QTimer();
    connect(this->freeze_timer_, SIGNAL(timeout()),
            this, SLOT(tickLiveReference()));

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
    if (isRed) {
        this->canvas_->path_graphic_->setColor(RED);
    } else {
        this->canvas_->path_graphic_->setColor(YELLOW);
    }
}

Controller::~Controller() {
    emit this->stopComputeWorker();
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
    delete this->freeze_timer_;
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
    switch (item->type()) {
        case POINT_GRAPHIC: {
            PointGraphicsItem *point = qgraphicsitem_cast<
                    PointGraphicsItem *>(item);
            PointModelItem *model = point->model_;
            if (this->model_->isCurrFinalPoint(model)) {
                this->model_->setCurrFinalPoint(nullptr);
            }
            this->removePointSocket(model);
            this->canvas_->removeItem(point);
            this->canvas_->final_points_.remove(point);
            delete point;
            this->model_->removePoint(model);
            delete model;
            break;
        }
        case ELLIPSE_GRAPHIC: {
            EllipseGraphicsItem *ellipse = qgraphicsitem_cast<
                    EllipseGraphicsItem *>(item);
            EllipseModelItem *model = ellipse->model_;
            this->removeEllipseSocket(model);
            this->canvas_->removeItem(ellipse);
            this->canvas_->ellipse_graphics_.remove(ellipse);
            delete ellipse;
            this->model_->removeEllipse(model);
            delete model;
            break;
        }
        case POLYGON_GRAPHIC: {
            PolygonGraphicsItem *polygon = qgraphicsitem_cast<
                    PolygonGraphicsItem *>(item);
            PolygonModelItem *model = polygon->model_;
            this->canvas_->removeItem(polygon);
            this->canvas_->polygon_graphics_.remove(polygon);
            delete polygon;
            this->model_->removePolygon(model);
            delete model;
            break;
        }
        case PLANE_GRAPHIC: {
            PlaneGraphicsItem *plane = qgraphicsitem_cast<
                    PlaneGraphicsItem *>(item);
            PlaneModelItem *model = plane->model_;
            this->canvas_->removeItem(plane);
            this->canvas_->plane_graphics_.remove(plane);
            delete plane;
            this->model_->removePlane(model);
            delete model;
            break;
        }
        case WAYPOINTS_GRAPHIC: {
            WaypointsResizeHandle *point_handle =
                    dynamic_cast<WaypointsResizeHandle *>(item);
            quint32 point_model_index = point_handle->index_;
            qgraphicsitem_cast<WaypointsGraphicsItem *>
                    (point_handle->parentItem())->
                    removeHandle(point_model_index);
            this->canvas_->removeItem(point_handle);
            delete point_handle;
            this->canvas_->waypoints_graphic_->expandScene();
            break;
        }
    }
}

void Controller::flipDirection(QGraphicsItem *item) {
    switch (item->type()) {
        case POLYGON_GRAPHIC: {
            PolygonGraphicsItem *polygon = qgraphicsitem_cast<
                    PolygonGraphicsItem *>(item);
            polygon->flipDirection();
            break;
        }
        case PLANE_GRAPHIC: {
            PlaneGraphicsItem *plane = qgraphicsitem_cast<
                    PlaneGraphicsItem *>(item);
            plane->flipDirection();
            break;
        }
    }
}

void Controller::addEllipse(QPointF const &point, qreal radius) {
    EllipseModelItem *item_model = new EllipseModelItem(point,
        this->model_->getClearance(), radius, radius, 0);
    this->loadEllipse(item_model);
    this->model_->updateEllipseColors();
}

void Controller::addPolygon(QVector<QPointF> points) {
    PolygonModelItem *item_model = new PolygonModelItem(points);
    this->loadPolygon(item_model);
}

void Controller::addPlane(QPointF const &p1, QPointF const &p2) {
    PlaneModelItem *item_model = new PlaneModelItem(p1, p2);
    this->loadPlane(item_model);
}

void Controller::addWaypoint(QPointF const &point) {
    this->model_->addWaypoint(point);
    this->canvas_->update();
}

void Controller::addFinalPoint(const QPointF &pos) {
    PointModelItem *item_model = new PointModelItem(pos);
    this->loadPoint(item_model);
}

void Controller::duplicateSelected() {
    for (QGraphicsItem *item : this->canvas_->selectedItems()) {
        switch (item->type()) {
            case ELLIPSE_GRAPHIC: {
                EllipseGraphicsItem *ellipse = qgraphicsitem_cast<
                        EllipseGraphicsItem *>(item);
                QPointF new_pos = QPointF(ellipse->model_->getPos());
                EllipseModelItem *new_model =
                        new EllipseModelItem(new_pos,
                                             this->model_->getClearance(),
                                             ellipse->model_->getHeight(),
                                             ellipse->model_->getWidth(),
                                             ellipse->model_->getRot());
                this->loadEllipse(new_model);
                break;
            }
        }
    }
}

// ============ BACK END CONTROLS ============

void Controller::freeze() {
    int msec = (1000 * this->model_->getFinaltime()) /
            (this->model_->getHorizon() - 1);
    this->freeze_timer_->start(msec);
    this->model_->setLiveReferenceMode(true);
    this->traj_index_ = 1;
}

void Controller::setStagedPath() {
    this->model_->setPathStagedPoints(this->model_->getPathPoints());
    this->model_->setIsTrajStaged(true);
    this->model_->setStagedTraj3dof(this->model_->getCurrTraj3dof());
    this->canvas_->path_staged_graphic_->setColor(GREEN);
    this->canvas_->path_staged_graphic_->expandScene();
}

void Controller::unsetStagedPath() {
    this->model_->clearPathStagedPoints();
    this->model_->setIsTrajStaged(false);
    this->canvas_->path_staged_graphic_->expandScene();
}

void Controller::tickLiveReference() {
    this->canvas_->path_staged_graphic_->expandScene();

    if (this->model_->tickPathStaged()) {
        autogen::packet::traj3dof traj = this->model_->getCurrTraj3dof();
//        this->canvas_->drone_graphic_->model_->
//                setPos(nedToGuiXyz(traj.pos_ned(0, this->traj_index_),
//                                   traj.pos_ned(1, this->traj_index_)));
        this->canvas_->drone_graphic_->model_->
                setVel(nedToGuiXyz(traj.vel_ned(0, this->traj_index_),
                                   traj.vel_ned(1, this->traj_index_)));
        this->canvas_->drone_graphic_->model_->
                setAccel(nedToGuiXyz(traj.accl_ned(0, this->traj_index_),
                                     traj.accl_ned(1, this->traj_index_)));
        this->traj_index_++;

        this->canvas_->drone_graphic_->expandScene();
    } else {
        this->freeze_timer_->stop();
        this->model_->setLiveReferenceMode(false);
        this->unsetStagedPath();
    }
}

void Controller::execute() {
    if (!this->freeze_timer_->isActive() &&
            this->model_->getIsTrajStaged()) {
        this->freeze();
        this->canvas_->path_staged_graphic_->setColor(CYAN);
        this->model_->setPathPoints(this->model_->getPathStagedPoints());
        emit trajectoryExecuted(this->model_->getStagedTraj3dof());
    }
}

void Controller::stageTraj() {
    if (!this->freeze_timer_->isActive() &&
            this->model_->getIsValidTraj() == FEASIBILITY_CODE::FEASIBLE) {
        this->setStagedPath();
    }
}

void Controller::unstageTraj() {
    if (!this->freeze_timer_->isActive()) {
        this->unsetStagedPath();
    }
}

void Controller::setPorts() {
    this->port_dialog_->setModel(this->model_);
    this->port_dialog_->open();
}

void Controller::setCanvas(Canvas *canvas) {
    this->canvas_ = canvas;
}

// ============ NETWORK CONTROLS ============

void Controller::startSockets() {
    // close old sockets
    this->closeSockets();

    // create drone socket
    if (this->canvas_->drone_graphic_->model_->port_ > 0) {
        this->drone_socket_ = new DroneSocket(
                    this->canvas_->drone_graphic_->model_);
        connect(this,
                SIGNAL(trajectoryExecuted(const autogen::packet::traj3dof)),
                this->drone_socket_,
                SLOT(rx_trajectory(const autogen::packet::traj3dof)));
        connect(this->drone_socket_, SIGNAL(refresh_graphics()),
                this->canvas_, SLOT(update()));
    }

    // create final pos socket
    for (PointGraphicsItem *graphic : this->canvas_->final_points_) {
        if (graphic->model_->port_ > 0) {
            PointSocket *temp = new PointSocket(graphic->model_);
            connect(temp, SIGNAL(refresh_graphics()),
                    this->canvas_, SLOT(update()));
            this->final_point_sockets_.append(temp);
        }
    }

    // create ellipse sockets
    for (EllipseGraphicsItem *graphic : this->canvas_->ellipse_graphics_) {
        if (graphic->model_->port_ > 0) {
            EllipseSocket *temp = new EllipseSocket(graphic->model_);
            connect(temp, SIGNAL(refresh_graphics()),
                    this->canvas_, SLOT(updateEllipseGraphicsItem(graphic)));
            this->ellipse_sockets_.append(temp);
        }
    }
}

void Controller::closeSockets() {
    if (this->drone_socket_) {
        delete this->drone_socket_;
        this->drone_socket_ = nullptr;
    }
    // close final point sockets
    for (PointSocket * socket : this->final_point_sockets_) {
        delete socket;
    }
    this->final_point_sockets_.clear();

    // close ellipse sockets
    for (EllipseSocket * socket : this->ellipse_sockets_) {
        delete socket;
    }
    this->ellipse_sockets_.clear();
}

void Controller::removeEllipseSocket(EllipseModelItem *model) {
    int index = 0;
    bool found = false;

    for (EllipseSocket *socket : this->ellipse_sockets_) {
        if (socket->ellipse_model_ == model) {
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
    int index = 0;
    bool found = false;

    for (PointSocket *socket : this->final_point_sockets_) {
        if (socket->point_model_ == model) {
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

// ============ LOAD CONTROLS ============

void Controller::loadEllipse(EllipseModelItem *item_model) {
    EllipseGraphicsItem *item_graphic =
            new EllipseGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->canvas_->ellipse_graphics_.insert(item_graphic);
    this->model_->addEllipse(item_model);
    item_graphic->setRotation(item_model->getRot());
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadPolygon(PolygonModelItem *item_model) {
    PolygonGraphicsItem *item_graphic =
            new PolygonGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->canvas_->polygon_graphics_.insert(item_graphic);
    this->model_->addPolygon(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadPlane(PlaneModelItem *item_model) {
    PlaneGraphicsItem *item_graphic =
            new PlaneGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->canvas_->plane_graphics_.insert(item_graphic);
    this->model_->addPlane(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadPoint(PointModelItem *item_model) {
    PointGraphicsItem *item_graphic =
            new PointGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->canvas_->final_points_.insert(item_graphic);
    this->model_->addPoint(item_model);
    item_graphic->setZValue(this->final_point_render_level_);
    item_graphic->expandScene();
}

}  // namespace optgui
