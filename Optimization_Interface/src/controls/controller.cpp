// TITLE:   Optimization_Interface/src/controls/controller.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "../../include/controls/controller.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QNetworkConfigurationManager>
#include <QSettings>
#include <QTranslator>
#include <QSet>

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>
#include <QElapsedTimer>

#include "../../include/graphics/point_graphics_item.h"
#include "../../include/graphics/ellipse_graphics_item.h"
#include "../../include/graphics/polygon_graphics_item.h"
#include "../../include/graphics/plane_graphics_item.h"
#include "../../include/network/drone_server.h"
#include "../../include/network/path_server.h"
#include "../../include/network/point_server.h"
#include "../../include/network/ellipse_server.h"
#include "../../include/network/polygon_server.h"
#include "../../include/network/plane_server.h"
#include "../../include/globals.h"
#include "../../include/window/menu_panel.h"

using namespace cprs;

namespace interface {

//constructor
Controller::Controller(Canvas *canvas, MenuPanel *menupanel) {
    this->canvas_ = canvas;
    this->menu_panel_ = menupanel;
    this->finaltime_ = menupanel->finaltime_init_;
    this->horizon_length_ = menupanel->horizonlength_init_;
    this->indoor_ = canvas->indoor_;
    this->model_ = new ConstraintModel(MAX_OBS, MAX_CPOS);

    this->network_session_ = nullptr;
    this->servers_ = new QVector<ItemServer *>();

    // initialize waypoints graphic
    this->waypoints_graphic_ = new WaypointsGraphicsItem(
                this->model_->waypoints_);
    this->canvas_->addItem(this->waypoints_graphic_);

    // initialize course graphic
    this->path_graphic_ = new PathGraphicsItem(this->model_->path_,
                                               nullptr,
                                               this->indoor_?4:16);
    this->canvas_->addItem(this->path_graphic_);

    // initialize drone graphic
    this->drone_graphic_ = new DroneGraphicsItem(this->model_->drone_,
                                                 nullptr,
                                                 this->indoor_?16:128);
    this->canvas_->addItem(this->drone_graphic_);

    this->loadEllipse(this->model_->puck_ellipse_pos_->at(0)); //TODO: make ellipse upate with puck symbol
    this->puck_graphic_ = new PointGraphicsItem(this->model_->puck_pos_->at(0),
                                                nullptr,
                                                this->indoor_?4:16);
    this->puck_graphic_->setMarker(1);
    this->canvas_->addItem(this->puck_graphic_);

    // initialize final point graphic
    this->final_pos_graphic_ = new PointGraphicsItem(this->model_->final_pos_,
                                                     nullptr,
                                                     this->indoor_?4:128);
    this->canvas_->addItem(this->final_pos_graphic_);

    // initialize port dialog
    this->port_dialog_ = new PortDialog();

    // TODO(ben): Remove this hack and incorporate it into ConstraintModel
    this->trajectory_ = new QVector<QPointF *>;

    // Hack: to add something on the four corners of the map
    this->bottom_left_ = new PointGraphicsItem(new PointModelItem(this->canvas_->getBottomLeft()));
    this->top_right_ = new PointGraphicsItem(new PointModelItem(this->canvas_->getTopRight()));

    this->canvas_->addItem(this->bottom_left_);
    this->canvas_->addItem(this->top_right_);

    this->drone_comm_ = new comm("", this->drone_port_);
    this->puck_comm_ = new comm("", this->puck_port_);

    connect(this, SIGNAL(trajectoryExecuted(const packet::traj3dof*)),
            this->drone_comm_, SLOT(rx_trajectory(const packet::traj3dof*)));
//    connect(this, SIGNAL(trajectoryExecuted2(const packet::traj3dof*)),
//            this->drone_comm_, SLOT(rx_trajectory2(const packet::traj3dof*)));
//    connect(this, SIGNAL(trajectoryExecuted2(float)),
//            this->drone_comm_, SLOT(rx_trajectory2(float)));
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
        case POINT_GRAPHIC: {
            PointGraphicsItem *point = qgraphicsitem_cast<
                    PointGraphicsItem *>(item);
            PointModelItem *model = point->model_;
            this->canvas_->removeItem(point);
            delete point;
            this->model_->removePoint(model);
            delete model;
            break;
        }
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

void Controller::updatePath() {
    //qDebug() << "Tick";
}

void Controller::setFinaltime(double_t finaltime) {
    this->finaltime_ = finaltime;
//    qDebug() << "Final time: " << finaltime;
    this->compute();
}

void Controller::setHorizonLength(uint32_t horizon) {
    this->horizon_length_ = horizon;
    qDebug() << "Horizon length: " << horizon;
    this->compute();
}

double_t Controller::getTimeInterval() {
    return this->finaltime_/this->horizon_length_;
}

void Controller::updateFinalPosition(QPointF *pos_final) {
    this->model_->final_pos_->pos_->setX(pos_final->x());
    this->model_->final_pos_->pos_->setY(pos_final->y());
    this->canvas_->update();
}

void Controller::compute() {
    if (this->isFrozen()) return;
    QVector<QPointF*> trajectory;
    this->clearPathPoints();

    this->compute(&trajectory);
    QVectorIterator<QPointF *> it(trajectory);
    while(it.hasNext()) {
                this->addPathPoint(it.next());
            }
}

void Controller::compute(QVector<QPointF *> *trajectory) {
    if (this->isFrozen()) return;
    this->timer_compute_.restart();

    // Initialize constant values
    // Parameters.
    //SKYENET// this should be moved to skyenet repo
    //
    /*

      SkyeAlg alg(this->horizon_length_);
      alg.setFinalTime(this->final_time_);

      alg.cpos.n = model_->loadEllipse(alg.P.obs.R, alg.P.obs.c_e, alg.obs.c_n);
      alg.cpos.n = model_->loadPosConstraint(alg.P.cpos.A, alg.P.cpos.b);

    double test = 5;
    SkyeFly fly;
    fly.setTimeHorizon(test); //fly.time_horizon = 5;
    fly.printTimeHorizon();
    */

    //Create flight trajectory object
    SkyeFly fly;

    //Parameters
    fly.P.K = MAX(MIN(this->horizon_length_, MAX_HORIZON), 5); // Number of points on the trajectory (resolution)
    fly.P.tf = this->finaltime_;   // duration of flight
    fly.P.dt = fly.P.tf/(fly.P.K-1.); // 'resolution'
    fly.P.obs.n = model_->loadEllipse(fly.P.obs.R, fly.P.obs.c_e, fly.P.obs.c_n); // Circle constraints \| H(r - p) \|^2 > R^2 where p is the center of the circle and R is the radius (H some linear transform)
    fly.P.cpos.n = model_->loadPosConstraint(fly.P.cpos.A, fly.P.cpos.b);  // Affine constraints Ax \leq b

    fly.P.dK = 1;
    fly.P.n_recalcs = 14;
    fly.P.g[0] = -9.81;
    fly.P.g[1] = 0.0;
    fly.P.g[2] = 0.0;
    fly.P.a_min = 5.0;   // Minimum accel.
    fly.P.a_max = 12.0; //  Maximum accel.
    fly.P.theta_max = 45.0*DEG2RAD;
    fly.P.q_max = 0.0;

    fly.P.max_iter = 10;
    fly.P.Delta_i = 100.0;
    fly.P.lambda = 1e2;
    fly.P.alpha = 2.0;
    fly.P.dL_tol = 1e-1;
    fly.P.rho_0 = -1e-1;
    fly.P.rho_1 = 0.25;
    fly.P.rho_2 = 0.90;
    fly.P.rirelax = 1000;
    fly.P.rfrelax = 10;

    // Inputs.
    this->model_->loadInitialPos(fly.I.r_i);
    fly.I.v_i[0] =  0.0;
    fly.I.v_i[1] =  0.0; //this->model_->drone_->vel_->x();
    fly.I.v_i[2] =  0.0; //this->model_->drone_->vel_->x();
    fly.I.a_i[0] = -fly.P.g[0];
    fly.I.a_i[1] = -fly.P.g[1];
    fly.I.a_i[2] = -fly.P.g[2];
    this->model_->loadFinalPos(fly.I.r_f);
    fly.I.v_f[0] =  0.0;
    fly.I.v_f[1] =  0.0;
    fly.I.v_f[2] =  0.0;
    fly.I.a_f[0] = -fly.P.g[0];
    fly.I.a_f[1] = -fly.P.g[1];
    fly.I.a_f[2] = -fly.P.g[2];


    // Initialize.
    fly.init();

    // Run SCvx algorithm
    fly.run();


    // outputs
    this->trajectory_->clear();
    for(uint32_t i=0; i<fly.P.K; i++) {
        trajectory->append(new QPointF(fly.O.r[2][i]*100, -fly.O.r[1][i]*100));
        this->trajectory_->append(new QPointF(fly.O.r[2][i]*100, -fly.O.r[1][i]*100));
    }

    // how feasible is the solution?
    // OUTPUT VIOLATIONS: constraint violation
    double accum = 0;
    for(uint32_t i=0; i<fly.P.K; i++) {
        accum += abs( pow(fly.O.a[0][i],2) + pow(fly.O.a[1][i],2) + pow(fly.O.a[2][i],2) \
                - pow(fly.O.s[i],2) )/fly.P.K;
    }

    // OUTPUT VIOLATIONS: initial and final pos violation
    accum = pow(fly.O.r_f_relax[0],2) + pow(fly.O.r_f_relax[1],2) + pow(fly.O.r_f_relax[2],2);

    if(accum > this->feasible_tol_) {
        this->valid_path_ = false;
        this->path_graphic_->setColor(QColor(Qt::red));
        this->menu_panel_->user_msg_label_->setText("Increase final time to regain feasibility!");

    } else {
        this->valid_path_ = true;
        this->path_graphic_->setColor(QColor(Qt::green));
        this->menu_panel_->user_msg_label_->setText("Trajectory remains feasible!");
    }

/* Debugging outputs
    qDebug() << "i= " << I.i
             << "| Del = " << O.Delta
             << "| L = " << O.L
             << "| J = " << O.J
             << "| dL = " << O.dL
             << "| dJ = " << O.dJ
             << "| I.J_0 = " << I.J_0
             << "| O.J = " << O.J
             << "| r = " << O.ratio;
    qDebug() << O.r_f_relax[0] << O.r_f_relax[1];
*/


    this->drone_traj3dof_data_.K = fly.P.K;
    for(quint32 k=0; k<fly.P.K; k++) {
//        this->drone_traj3dof_data_.clock_angle(k) = 90.0/180.0*3.141592*P.dt*k;

        this->drone_traj3dof_data_.time(k) = k*fly.P.dt;
        this->drone_traj3dof_data_.pos_ned(0,k) = fly.O.r[1][k];
        this->drone_traj3dof_data_.pos_ned(1,k) = fly.O.r[2][k];
        this->drone_traj3dof_data_.pos_ned(2,k) = fly.O.r[0][k];

        this->drone_traj3dof_data_.vel_ned(0,k) = fly.O.v[1][k];
        this->drone_traj3dof_data_.vel_ned(1,k) = fly.O.v[2][k];
        this->drone_traj3dof_data_.vel_ned(2,k) = fly.O.v[0][k];

        this->drone_traj3dof_data_.accl_ned(0,k) = fly.O.a[1][k];
        this->drone_traj3dof_data_.accl_ned(1,k) = fly.O.a[2][k];
        this->drone_traj3dof_data_.accl_ned(2,k) = fly.O.a[0][k] - 9.81;
    }
//    this->drone_traj3dof_data_.
    // Set up next solution.
    //SKYEFLY// fly.reset
    reset(fly.P,fly.I,fly.O);
    qInfo() << "Solver took " << this->timer_compute_.elapsed() << "ms";
    this->solver_difficulty_ = this->timer_compute_.elapsed();
}

bool Controller::isFrozen() {
    bool frozen = exec_once_ && timer_exec_.elapsed()/1000 <= this->finaltime_*1.2;
    if(frozen) {
        qInfo() << "Frozen!";
    }
    return frozen;
}

void Controller::execute() {
    if (!this->valid_path_) {
        qInfo() << "Execution disabled, no valid trajectory.";
        return;
    }
    if (this->isFrozen()) {
        std::cout << "Execution disabled, frozen mode.";
        return;
    }
    std::cout << "Executing trajectory!";
    qDebug() << "Executing trajectory..";
    this->exec_once_ = true;
    timer_exec_.restart();

    std::cout << "pos:" << this->drone_traj3dof_data_.pos_ned.transpose() << std::endl;
    std::cout << "vel:" << this->drone_traj3dof_data_.vel_ned.transpose() << std::endl;
    std::cout << "accl:" << this->drone_traj3dof_data_.accl_ned.transpose() << std::endl;


    qDebug() << "before";
    emit trajectoryExecuted(&this->drone_traj3dof_data_);
    qDebug() << "after";
}

bool Controller::simDrone(uint64_t tick) {
    if(tick >= (uint64_t)this->trajectory_->length())
        return false;
    else {
        this->updateDronePos(*this->trajectory_->value(tick));
    }
    return true;
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

void Controller::updatePuckPos(uint32_t idx, QPointF pos) {
    this->model_->puck_pos_->at(idx)->pos_->setX(pos.x());
    this->model_->puck_pos_->at(idx)->pos_->setY(pos.y());
    this->model_->puck_ellipse_pos_->at(idx)->pos_->setX(pos.x());
    this->model_->puck_ellipse_pos_->at(idx)->pos_->setY(pos.y());
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

    // Create servers for point constraints
    for (PointModelItem *model : *this->model_->points_) {
        if (model->port_ != 0) {
            this->servers_->append(new PointServer(model));
        }
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

void Controller::writePoint(PointModelItem *model, QDataStream *out) {
    *out << (bool)model->direction_;
    *out << *model->pos_;
//    *out << (double)model->radius_;
    *out << (quint16)model->port_;
}

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
        out->setVersion(VERSION_5_8);

        // Write points
        quint32 num_points = this->model_->points_->size();
        *out << num_points;
        for (PointModelItem *model : *this->model_->points_) {
            this->writePoint(model, out);
        }

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

// NOT being used
void Controller::loadPoint(PointModelItem *item_model) {
//    PointGraphicsItem *test = new PointGraphicsItem();
    PointGraphicsItem *item_graphic = new PointGraphicsItem(item_model);

    this->canvas_->addItem(item_graphic);
    this->model_->addPoint(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadEllipse(EllipseModelItem *item_model) {
    EllipseGraphicsItem *item_graphic = new EllipseGraphicsItem(item_model,
                                                                nullptr,
                                                                this->indoor_?16:128);
    this->canvas_->addItem(item_graphic);
    this->model_->addEllipse(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadPolygon(PolygonModelItem *item_model) {
    PolygonGraphicsItem *item_graphic = new PolygonGraphicsItem(item_model,
                                                                nullptr,
                                                                this->indoor_?16:128);
    this->canvas_->addItem(item_graphic);
    this->model_->addPolygon(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadPlane(PlaneModelItem *item_model) {
    PlaneGraphicsItem *item_graphic = new PlaneGraphicsItem(item_model,
                                                            nullptr,
                                                            this->indoor_?16:128);
    this->canvas_->addItem(item_graphic);
    this->model_->addPlane(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

PointModelItem *Controller::readPoint(QDataStream *in) {
    bool direction;
    *in >> direction;
    QPointF pos;
    *in >> pos;
//    double radius;
//    *in >> radius;
    quint16 port;
    *in >> port;

    PointModelItem *model = new PointModelItem(new QPointF(pos));
    model->direction_ = direction;
//    model->radius_ = radius;
    model->port_ = port;

    return model;
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
        in->setVersion(VERSION_5_8);

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
