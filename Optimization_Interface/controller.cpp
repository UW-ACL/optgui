// TITLE:   Optimization_Interface/controller.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "controller.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QNetworkConfigurationManager>
#include <QSettings>
#include <QTranslator>

#include "ellipse_graphics_item.h"
#include "polygon_graphics_item.h"
#include "plane_graphics_item.h"
#include "drone_server.h"
#include "path_server.h"
#include "ellipse_server.h"
#include "polygon_server.h"
#include "plane_server.h"
#include "globals.h"

namespace interface {

Controller::Controller(Canvas *canvas) {
    this->canvas_ = canvas;
    this->model_ = new ConstraintModel();

    this->network_session_ = nullptr;
    this->servers_ = new QVector<ItemServer *>();

    // initialize waypoints graphic
    this->waypoints_graphic_ = new WaypointsGraphicsItem(
                this->model_->waypoints_);
    this->canvas_->addItem(this->waypoints_graphic_);

    // initialize course graphic
    this->path_graphic_ = new PathGraphicsItem(
                this->model_->path_);
    this->canvas_->addItem(this->path_graphic_);

    // initialize drone graphic
    this->drone_graphic_ = new DroneGraphicsItem(this->model_->drone_);
    this->canvas_->addItem(this->drone_graphic_);

    // initialize port dialog
    this->port_dialog_ = new PortDialog();
}

Controller::~Controller() {
    // deinitialize waypoints graphic
    this->clearWaypointsGraphic();
    delete this->waypoints_graphic_;

    // deinitialize path graphic
    this->clearPathGraphic();
    delete this->path_graphic_;

    // deinitialize drone graphic
    this->clearDroneGraphic();
    delete this->drone_graphic_;

    // deinitialize port dialog
    delete this->port_dialog_;

    // deinitialize network session
    if (this->network_session_ != nullptr) {
        delete this->network_session_;
    }

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
            this->canvas_->removeItem(ellipse);
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
            delete plane;
            this->model_->removePlane(model);
            delete model;
            break;
        }
        case HANDLE_GRAPHIC: {
            if (item->parentItem() &&
                    item->parentItem()->type() == WAYPOINTS_GRAPHIC) {
                PolygonResizeHandle *point_handle =
                        dynamic_cast<PolygonResizeHandle *>(item);
                QPointF *point_model = point_handle->getPoint();
                qgraphicsitem_cast<WaypointsGraphicsItem *>
                        (point_handle->parentItem())->
                        removeHandle(point_handle);
                this->canvas_->removeItem(point_handle);
                delete point_handle;
                this->model_->removeWaypoint(point_model);
                this->canvas_->update();
                delete point_model;
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

void Controller::addEllipse(QPointF *point) {
    EllipseModelItem *item_model = new EllipseModelItem(point);
    this->loadEllipse(item_model);
}

void Controller::addPolygon(QVector<QPointF *> *points) {
    PolygonModelItem *item_model = new PolygonModelItem(points);
    this->loadPolygon(item_model);
}

void Controller::addPlane(QPointF *p1, QPointF *p2) {
    PlaneModelItem *item_model = new PlaneModelItem(p1, p2);
    this->loadPlane(item_model);
}

void Controller::addWaypoint(QPointF *point) {
    this->model_->addWaypoint(point);
    this->canvas_->update();
}

// ============ BACK END CONTROLS ============

void Controller::execute() {
    // TODO(Miki): pass model to optimization solver
}

void Controller::setPorts() {
    this->port_dialog_->setModel(this->model_);
    this->port_dialog_->open();
}

void Controller::clearWaypointsGraphic() {
    this->canvas_->removeItem(this->waypoints_graphic_);
}

void Controller::clearPathGraphic() {
    this->canvas_->removeItem(this->path_graphic_);
}

void Controller::clearDroneGraphic() {
    this->canvas_->removeItem(this->drone_graphic_);
}

void Controller::setCanvas(Canvas *canvas) {
    this->canvas_ = canvas;
}

void Controller::addPathPoint(QPointF *point) {
    this->model_->addPathPoint(point);
    this->canvas_->update();
}

void Controller::clearPathPoints() {
    this->model_->clearPath();
    this->canvas_->update();
}

void Controller::updateDronePos(QPointF pos) {
    this->model_->drone_->point_->setX(pos.x());
    this->model_->drone_->point_->setY(pos.y());
    this->canvas_->update();
}

// ============ NETWORK CONTROLS ============

void Controller::startServers() {
    // Close and clear existing servers
    this->closeServers();

    // Get network config
    QNetworkConfigurationManager manager;
    if (manager.capabilities() &
            QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope,
                           QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id =
                settings.value(
                    QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently
        // discovered use the system default
        QNetworkConfiguration config =
                manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        if (this->network_session_) {
            this->network_session_->stop();
            delete this->network_session_;
        }
        this->network_session_ = new QNetworkSession(config);
        network_session_->open();
    }

    // Save the used configuration
    if (this->network_session_) {
        QNetworkConfiguration config = this->network_session_->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice) {
            id = this->network_session_->sessionProperty(
                        QLatin1String("UserChoiceConfiguration")).toString();
        } else {
            id = config.identifier();
        }

        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
    }

    // Create server for drone
    if (this->model_->drone_->port_ != 0) {
        this->servers_->append(new DroneServer(this->model_->drone_));
    }

    // Create server for waypoints
    if (this->model_->waypoints_->port_ != 0) {
        this->servers_->append(new PathServer(this->model_->waypoints_));
    }

    // Create server for drone path
    if (this->model_->path_->port_ != 0) {
        this->servers_->append(new PathServer(this->model_->path_));
    }

    // Create servers for ellipse constraints
    for (EllipseModelItem *model : *this->model_->ellipses_) {
        if (model->port_ != 0) {
            this->servers_->append(new EllipseServer(model));
        }
    }

    // Create servers for polygon constraints
    for (PolygonModelItem *model : *this->model_->polygons_) {
        if (model->port_ != 0) {
            this->servers_->append(new PolygonServer(model));
        }
    }

    // Create servers for plane constraints
    for (PlaneModelItem *model : *this->model_->planes_) {
        if (model->port_ != 0) {
            this->servers_->append(new PlaneServer(model));
        }
    }

    // Start servers
    for (ItemServer *server : *this->servers_) {
        server->startServer();
    }
}

void Controller::closeServers() {
    // close servers in list
    for (ItemServer* server : *this->servers_) {
        server->close();
        delete server;
    }
    this->servers_->clear();

    // close network session
    if (this->network_session_) {
        this->network_session_->close();
    }
}

// ============ SAVE CONTROLS ============

void Controller::writeEllipse(EllipseModelItem *model, QDataStream *out) {
    *out << (bool)model->direction_;
    *out << *model->pos_;
    *out << (double)model->radius_;
    *out << (quint16)model->port_;
}

void Controller::writePolygon(PolygonModelItem *model, QDataStream *out) {
    *out << (bool)model->direction_;
    *out << (quint32)model->points_->size();
    for (QPointF *point : *model->points_) {
        *out << *point;
    }
    *out << (quint16)model->port_;
}

void Controller::writePlane(PlaneModelItem *model, QDataStream *out) {
    *out << (bool)model->direction_;
    *out << *model->p1_;
    *out << *model->p2_;
    *out << (quint16)model->port_;
}

void Controller::writeWaypoints(PathModelItem *model, QDataStream *out) {
    quint32 num_waypoints = model->points_->size();
    *out << num_waypoints;
    for (QPointF *point : *model->points_) {
        *out << *point;
    }
    *out << (quint16)model->port_;
}

void Controller::writePath(PathModelItem *model, QDataStream *out) {
    quint32 num_path_points = model->points_->size();
    *out << num_path_points;
    for (QPointF *point : *model->points_) {
        *out << *point;
    }
    *out << (quint16)model->port_;
}

void Controller::writeDrone(DroneModelItem *model, QDataStream *out) {
    *out << *model->point_;
    *out << (quint16)model->port_;
}

void Controller::saveFile() {
    QString file_name = QFileDialog::getSaveFileName(nullptr,
        QFileDialog::tr("Save Constraint Layout"), "",
        QFileDialog::tr("Constraint Layout (*.cst);;All Files (*)"));

    if (file_name.isEmpty()) {
        return;
    } else {
        QFile *file = new QFile(file_name);
        if (!file->open(QIODevice::WriteOnly)) {
            QMessageBox::information(nullptr,
                                     QFileDialog::tr("Unable to open file"),
                                     file->errorString());
            delete file;
            return;
        }

        QDataStream *out = new QDataStream(file);
        out->setVersion(QDataStream::Qt_5_11);

        // Write ellipses
        quint32 num_ellipses = this->model_->ellipses_->size();
        *out << num_ellipses;
        for (EllipseModelItem *model : *this->model_->ellipses_) {
            this->writeEllipse(model, out);
        }

        // Write polygons
        quint32 num_polygons = this->model_->polygons_->size();
        *out << num_polygons;
        for (PolygonModelItem *model : *this->model_->polygons_) {
            this->writePolygon(model, out);
        }

        // Write planes
        quint32 num_planes = this->model_->planes_->size();
        *out << num_planes;
        for (PlaneModelItem *model : *this->model_->planes_) {
            this->writePlane(model, out);
        }

        // Write waypoints
        this->writeWaypoints(this->model_->waypoints_, out);

        // Write drone
        this->writeDrone(this->model_->drone_, out);

        // Write drone path
        this->writePath(this->model_->path_, out);

        // Clean up IO
        delete out;
        delete file;
    }
}

// ============ LOAD CONTROLS ============

void Controller::loadEllipse(EllipseModelItem *item_model) {
    EllipseGraphicsItem *item_graphic = new EllipseGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->model_->addEllipse(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadPolygon(PolygonModelItem *item_model) {
    PolygonGraphicsItem *item_graphic = new PolygonGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->model_->addPolygon(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadPlane(PlaneModelItem *item_model) {
    PlaneGraphicsItem *item_graphic = new PlaneGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->model_->addPlane(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

EllipseModelItem *Controller::readEllipse(QDataStream *in) {
    bool direction;
    *in >> direction;
    QPointF pos;
    *in >> pos;
    double radius;
    *in >> radius;
    quint16 port;
    *in >> port;

    EllipseModelItem *model = new EllipseModelItem(new QPointF(pos));
    model->direction_ = direction;
    model->radius_ = radius;
    model->port_ = port;

    return model;
}

PolygonModelItem *Controller::readPolygon(QDataStream *in) {
    bool direction;
    *in >> direction;
    quint32 size;
    *in >> size;

    QVector<QPointF *> *points = new QVector<QPointF *>();
    points->reserve(size);
    for (quint32 i = 0; i < size; i++) {
        QPointF point;
        *in >> point;
        points->append(new QPointF(point));
    }
    PolygonModelItem *model = new PolygonModelItem(points);
    model->direction_ = direction;

    quint16 port;
    *in >> port;
    model->port_ = port;

    return model;
}

PlaneModelItem *Controller::readPlane(QDataStream *in) {
    bool direction;
    *in >> direction;
    QPointF p1;
    *in >> p1;
    QPointF p2;
    *in >> p2;
    quint16 port;
    *in >> port;

    PlaneModelItem *model = new PlaneModelItem(new QPointF(p1),
                                               new QPointF(p2));
    model->direction_ = direction;
    model->port_ = port;

    return model;
}

void Controller::readWaypoints(QDataStream *in) {
    quint32 num_waypoints;
    *in >> num_waypoints;
    for (quint32 i = 0; i < num_waypoints; i++) {
        QPointF point;
        *in >> point;
        this->addWaypoint(new QPointF(point));
    }
    quint16 port;
    *in >> port;
    this->model_->waypoints_->port_ = port;
}

void Controller::readPath(QDataStream *in) {
    quint32 num_path_points;
    *in >> num_path_points;
    for (quint32 i = 0; i < num_path_points; i++) {
        QPointF point;
        *in >> point;
        this->addPathPoint(new QPointF(point));
    }
    quint16 port;
    *in >> port;
    this->model_->path_->port_ = port;
}

void Controller::readDrone(QDataStream *in) {
    QPointF point;
    *in >> point;
    quint16 port;
    *in >> port;
    this->updateDronePos(point);
    this->model_->drone_->port_ = port;
}

void Controller::loadFile() {
    QString file_name = QFileDialog::getOpenFileName(nullptr,
        QFileDialog::tr("Open Constraint Layout"), "",
        QFileDialog::tr("Constraint Layout (*.cst);;All Files (*)"));

    if (file_name.isEmpty()) {
        return;
    } else {
        QFile *file = new QFile(file_name);

        if (!file->open(QIODevice::ReadOnly)) {
            QMessageBox::information(nullptr,
                                     QFileDialog::tr("Unable to open file"),
                                     file->errorString());
            delete file;
            return;
        }

        QDataStream *in = new QDataStream(file);
        in->setVersion(QDataStream::Qt_5_11);

        // Reset model
        delete this->model_;
        this->model_ = new ConstraintModel();

        // Reset waypoints graphic
        delete this->waypoints_graphic_;
        this->waypoints_graphic_ = new WaypointsGraphicsItem(
                    this->model_->waypoints_);
        this->canvas_->addItem(this->waypoints_graphic_);

        // Reset path graphic
        delete this->path_graphic_;
        this->path_graphic_ =
                new PathGraphicsItem(this->model_->path_);
        this->canvas_->addItem(this->path_graphic_);

        // Reset drone graphic
        delete this->drone_graphic_;
        this->drone_graphic_ =
                new DroneGraphicsItem(this->model_->drone_);
        this->canvas_->addItem(this->drone_graphic_);

        // Read ellipses
        quint32 num_ellipses;
        *in >> num_ellipses;
        for (quint32 i = 0; i < num_ellipses; i++) {
            this->loadEllipse(this->readEllipse(in));
        }

        // Read polygons
        quint32 num_polygons;
        *in >> num_polygons;
        for (quint32 i = 0; i < num_polygons; i++) {
            this->loadPolygon(this->readPolygon(in));
        }

        // Read planes
        quint32 num_planes;
        *in >> num_planes;
        for (quint32 i = 0; i < num_planes; i++) {
            this->loadPlane(this->readPlane(in));
        }

        // Read waypoints
        this->readWaypoints(in);

        // Read drone
        this->readDrone(in);

        // Read path
        this->readPath(in);

        // clean up IO
        delete in;
        delete file;
    }
}

}  // namespace interface
