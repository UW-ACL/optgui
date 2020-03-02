// TITLE:   Optimization_Interface/src/controls/controller.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/controls/controller.h"

#include <QMessageBox>
#include <QSettings>
#include <QTranslator>
#include <QSet>

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

Controller::Controller(Canvas *canvas, MenuPanel *menupanel) {
    this->canvas_ = canvas;
    this->model_ = new ConstraintModel();

    // initialize waypoints model and graphic
    PathModelItem *waypoint_model = new PathModelItem();
    this->model_->setWaypointsModel(waypoint_model);
    this->canvas_->waypoints_graphic_ =
            new WaypointsGraphicsItem(waypoint_model);
    this->canvas_->addItem(this->canvas_->waypoints_graphic_);

    // initialize trajectory model and graphic
    PathModelItem *trajectory_model = new PathModelItem();
    this->model_->setPathModel(trajectory_model);
    this->canvas_->path_graphic_ =
            new PathGraphicsItem(trajectory_model);
    this->canvas_->addItem(this->canvas_->path_graphic_);

    // initialize drone model and graphic
    DroneModelItem *drone_model = new DroneModelItem();
    this->model_->setDroneModel(drone_model);
    this->canvas_->drone_graphic_ =
            new DroneGraphicsItem(drone_model);
    this->canvas_->addItem(this->canvas_->drone_graphic_);

    // initialize final point model and graphic
    PointModelItem *final_point_model = new PointModelItem();
    this->model_->setFinalPointModel(final_point_model);
    this->canvas_->final_point_ =
            new PointGraphicsItem(final_point_model);
    this->canvas_->addItem(this->canvas_->final_point_);

    // initialize port dialog
    this->port_dialog_ = new PortDialog();
    connect(this->port_dialog_, SIGNAL(setSocketPorts()),
            this, SLOT(startSockets()));

    // Initialize network
    this->drone_socket_ = nullptr;
    this->final_point_socket_ = nullptr;
    this->ellipse_sockets_ = new QVector<EllipseSocket *>();

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
        this->canvas_->path_graphic_->setColor(Qt::red);
    } else {
        this->canvas_->path_graphic_->setColor(Qt::green);
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
    delete this->ellipse_sockets_;

    // clean up model
    delete this->model_;
}

// ============ MOUSE CONTROLS ============

void Controller::removeItem(QGraphicsItem *item) {
    switch (item->type()) {
        case ELLIPSE_GRAPHIC: {
            EllipseGraphicsItem *ellipse = qgraphicsitem_cast<
                    EllipseGraphicsItem *>(item);
            EllipseModelItem *model = ellipse->model_;
            this->removeEllipseSocket(model);
            this->canvas_->removeItem(ellipse);
            this->canvas_->ellipse_graphics_->remove(ellipse);
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
            this->canvas_->polygon_graphics_->remove(polygon);
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
            this->canvas_->plane_graphics_->remove(plane);
            delete plane;
            this->model_->removePlane(model);
            delete model;
            break;
        }
        case HANDLE_GRAPHIC: {
            if (item->parentItem() &&
                    item->parentItem()->type() == WAYPOINTS_GRAPHIC) {
                WaypointsResizeHandle *point_handle =
                        reinterpret_cast<WaypointsResizeHandle *>(item);
                quint32 point_model_index = point_handle->index_;
                qgraphicsitem_cast<WaypointsGraphicsItem *>
                        (point_handle->parentItem())->
                        removeHandle(point_model_index);
                this->canvas_->removeItem(point_handle);
                delete point_handle;
                this->canvas_->waypoints_graphic_->expandScene();
            }
            break;
        }
    }
}

void Controller::flipDirection(QGraphicsItem *item) {
    switch (item->type()) {
        case ELLIPSE_GRAPHIC: {
            EllipseGraphicsItem *ellipse = qgraphicsitem_cast<
                    EllipseGraphicsItem *>(item);
            ellipse->flipDirection();
            break;
        }
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

void Controller::addEllipse(QPointF point, qreal radius) {
    EllipseModelItem *item_model = new EllipseModelItem(point, radius);
    this->loadEllipse(item_model);
}

void Controller::addPolygon(QVector<QPointF> points) {
    PolygonModelItem *item_model = new PolygonModelItem(points);
    this->loadPolygon(item_model);
}

void Controller::addPlane(QPointF p1, QPointF p2) {
    PlaneModelItem *item_model = new PlaneModelItem(p1, p2);
    this->loadPlane(item_model);
}

void Controller::addWaypoint(QPointF point) {
    this->model_->addWaypoint(point);
    this->canvas_->update();
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
                                             ellipse->model_->getRadius());
                this->loadEllipse(new_model);
                break;
            }
        }
    }
}

// ============ BACK END CONTROLS ============

void Controller::updateFinalPosition(QPointF const &pos) {
    this->model_->setFinalPointPos(pos);
}

void Controller::execute() {
    if (this->model_->getIsValidTraj()) {
        emit trajectoryExecuted(this->model_->getTraj3dof());
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
    if (this->canvas_->final_point_->model_->port_ > 0) {
        this->final_point_socket_ = new PointSocket(
                    this->canvas_->final_point_->model_);
        connect(this->final_point_socket_, SIGNAL(refresh_graphics()),
                this->canvas_, SLOT(update()));
    }

    // create ellipse sockets
    for (EllipseGraphicsItem *graphic : *this->canvas_->ellipse_graphics_) {
        if (graphic->model_->port_ > 0) {
            EllipseSocket *temp = new EllipseSocket(graphic->model_);
            connect(temp, SIGNAL(refresh_graphics()),
                    this->canvas_, SLOT(updateEllipseGraphicsItem(graphic)));
            this->ellipse_sockets_->append(temp);
        }
    }
}

void Controller::closeSockets() {
    if (this->drone_socket_) {
        delete this->drone_socket_;
        this->drone_socket_ = nullptr;
    }
    if (this->final_point_socket_) {
        delete this->final_point_socket_;
        this->final_point_socket_ = nullptr;
    }
    // close ellipse sockets
    while (!this->ellipse_sockets_->isEmpty()) {
        delete this->ellipse_sockets_->takeFirst();
    }
}

void Controller::removeEllipseSocket(EllipseModelItem *model) {
    for (QVector<EllipseSocket *>::iterator i = this->ellipse_sockets_->begin();
         i != this->ellipse_sockets_->end(); i++) {
        if ((*i)->ellipse_model_ == model) {
            EllipseSocket *temp = *i;
            i = this->ellipse_sockets_->erase(i);
            delete temp;
        }
    }
}

// ============ LOAD CONTROLS ============

void Controller::loadEllipse(EllipseModelItem *item_model) {
    EllipseGraphicsItem *item_graphic =
            new EllipseGraphicsItem(item_model, nullptr);
    this->canvas_->addItem(item_graphic);
    this->canvas_->ellipse_graphics_->insert(item_graphic);
    this->model_->addEllipse(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadPolygon(PolygonModelItem *item_model) {
    PolygonGraphicsItem *item_graphic =
            new PolygonGraphicsItem(item_model, nullptr);
    this->canvas_->addItem(item_graphic);
    this->canvas_->polygon_graphics_->insert(item_graphic);
    this->model_->addPolygon(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadPlane(PlaneModelItem *item_model) {
    PlaneGraphicsItem *item_graphic =
            new PlaneGraphicsItem(item_model, nullptr);
    this->canvas_->addItem(item_graphic);
    this->canvas_->plane_graphics_->insert(item_graphic);
    this->model_->addPlane(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

}  // namespace optgui
