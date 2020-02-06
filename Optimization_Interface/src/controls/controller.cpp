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
#include "include/globals.h"
#include "include/window/menu_panel.h"

namespace interface {

Controller::Controller(Canvas *canvas, MenuPanel *menupanel) {
    this->canvas_ = canvas;
    this->menu_panel_ = menupanel;
    this->indoor_ = canvas->indoor_;
    this->model_ = new ConstraintModel(MAX_OBS, MAX_CPOS);

    // initialize waypoints graphic
    this->canvas_->waypoints_graphic_ =
            new WaypointsGraphicsItem(this->model_->waypoints_);
    this->canvas_->addItem(this->canvas_->waypoints_graphic_);

    // initialize course graphic
    this->canvas_->path_graphic_ =
            new PathGraphicsItem(this->model_->path_);
    this->canvas_->addItem(this->canvas_->path_graphic_);

    // initialize drone graphic
    this->canvas_->drone_graphic_ =
            new DroneGraphicsItem(this->model_->drone_);
    this->canvas_->addItem(this->canvas_->drone_graphic_);

    // initialize final point graphic
    this->canvas_->final_point_ =
            new PointGraphicsItem(this->model_->final_pos_);
    this->canvas_->addItem(this->canvas_->final_point_);

    // initialize port dialog
    this->port_dialog_ = new PortDialog();
    connect(this->port_dialog_, SIGNAL(setSocketPorts()),
            this, SLOT(startSockets()));


    // Start compute callback
    connect(this, SIGNAL(startCompute()), this, SLOT(compute()));

    // Initialize network
    this->drone_socket_ = nullptr;
    this->final_point_socket_ = nullptr;
    this->ellipse_sockets_ = new QVector<EllipseSocket *>();

//    emit startCompute();
//    QTimer* activeTimer = new QTimer(this);
//    activeTimer->setInterval(20000);
//    activeTimer->setSingleShot(false);
//    connect(activeTimer, SIGNAL(timeout()), this, SLOT(compute()));
//    activeTimer->start();
}

Controller::~Controller() {
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

void Controller::addEllipse(QPointF *point, qreal radius) {
    EllipseModelItem *item_model = new EllipseModelItem(point, radius);
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

void Controller::duplicateSelected() {
    for (QGraphicsItem *item : this->canvas_->selectedItems()) {
        switch (item->type()) {
            case ELLIPSE_GRAPHIC: {
                EllipseGraphicsItem *ellipse = qgraphicsitem_cast<
                        EllipseGraphicsItem *>(item);
                QPointF *new_pos = new QPointF(ellipse->model_->pos_->x(),
                                               ellipse->model_->pos_->y());
                EllipseModelItem *new_model =
                        new EllipseModelItem(new_pos,
                                             ellipse->model_->radius_);
                this->loadEllipse(new_model);
                break;
            }
        }
    }
}

// ============ BACK END CONTROLS ============

void Controller::setFinaltime(double finaltime) {
    this->model_->finaltime_ = finaltime;
}

void Controller::setHorizonLength(uint32_t horizon) {
    this->model_->horizon_length_ = horizon;
}

double Controller::getTimeInterval() {
    return this->model_->finaltime_ / this->model_->horizon_length_;
}

void Controller::updateFinalPosition(QPointF const &pos) {
    this->model_->final_pos_->pos_->setX(pos.x());
    this->model_->final_pos_->pos_->setY(pos.y());
    this->canvas_->final_point_->setPos(*this->model_->final_pos_->pos_);
}

//void Controller::compute() {
//    if (this->isFrozen()) return;
//    //
//    QVector<QPointF*> trajectory;
//    this->clearPathPoints();

//    this->compute(&trajectory);
//    QVectorIterator<QPointF *> it(trajectory);
//    while (it.hasNext()) {
//        this->addPathPoint(it.next());
//    }
//}

void Controller::compute() { //QVector<QPointF *> *trajectory) {

    // Initialize constant values
    // Parameters.
    // SKYENET // this should be moved to skyenet repo
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

    // Parameters
    // Number of points on the trajectory (resolution)
    this->model_->fly_->P.K = MAX(MIN(
            this->model_->horizon_length_, MAX_HORIZON), 5);
    this->model_->fly_->P.tf = this->model_->finaltime_;   // duration of flight
    this->model_->fly_->P.dt = this->model_->fly_->P.tf
                              / (this->model_->fly_->P.K-1.);  // 'resolution'
    // Circle constraints \| H(r - p) \|^2 > R^2 where p is the center of the
    // circle and R is the radius (H some linear transform)
    this->model_->fly_->P.obs.n = model_->
            loadEllipseConstraint(this->model_->fly_->P.obs.R,
                        this->model_->fly_->P.obs.c_e,
                        this->model_->fly_->P.obs.c_n);
    // Affine constraints Ax \leq b
    this->model_->fly_->P.cpos.n = model_->
            loadPosConstraints(this->model_->fly_->P.cpos.A,
                              this->model_->fly_->P.cpos.b);

    // Inputs.
    this->model_->loadInitialPos(this->model_->fly_->I.r_i);
    this->model_->fly_->I.v_i[0] =  0.0;
    this->model_->fly_->I.v_i[1] =  0.0;  // this->model_->drone_->vel_->x();
    this->model_->fly_->I.v_i[2] =  0.0;  // this->model_->drone_->vel_->x();
    this->model_->fly_->I.a_i[0] = -this->model_->fly_->P.g[0];
    this->model_->fly_->I.a_i[1] = -this->model_->fly_->P.g[1];
    this->model_->fly_->I.a_i[2] = -this->model_->fly_->P.g[2];
    this->model_->loadFinalPos(this->model_->fly_->I.r_f);
    this->model_->fly_->I.v_f[0] =  0.0;
    this->model_->fly_->I.v_f[1] =  0.0;
    this->model_->fly_->I.v_f[2] =  0.0;
    this->model_->fly_->I.a_f[0] = -this->model_->fly_->P.g[0];
    this->model_->fly_->I.a_f[1] = -this->model_->fly_->P.g[1];
    this->model_->fly_->I.a_f[2] = -this->model_->fly_->P.g[2];


    // Initialize.
    this->model_->fly_->init();

    // Run SCvx algorithm
    this->model_->fly_->run();

    // outputs
    this->clearPathPoints();
//    this->model_->trajectory_->clear();

    for (uint32_t i = 0; i < this->model_->fly_->P.K; i++) {
        // TODO: Look into memory leak
        this->addPathPoint(new QPointF(this->model_->fly_->O.r[2][i] * GRID_SIZE,
                                -this->model_->fly_->O.r[1][i] * GRID_SIZE));

    }

    // how feasible is the solution?
    // OUTPUT VIOLATIONS: constraint violation
    double accum = 0;
    for (uint32_t i = 0; i < this->model_->fly_->P.K; i++) {
        accum += abs(pow(this->model_->fly_->O.a[0][i], 2)
                     + pow(this->model_->fly_->O.a[1][i], 2)
                     + pow(this->model_->fly_->O.a[2][i], 2)
                     - pow(this->model_->fly_->O.s[i], 2))
                 / this->model_->fly_->P.K;
    }

    // OUTPUT VIOLATIONS: initial and final pos violation
    accum = pow(this->model_->fly_->O.r_f_relax[0], 2)
            + pow(this->model_->fly_->O.r_f_relax[1], 2)
            + pow(this->model_->fly_->O.r_f_relax[2], 2);

    if (accum > 0.25) {
        this->valid_path_ = false;
        this->canvas_->path_graphic_->setColor(QColor(Qt::red));
        this->menu_panel_->user_msg_label_->
                setText("Increase final time to regain feasibility!");

    } else {
        this->valid_path_ = true;
        this->canvas_->path_graphic_->setColor(QColor(Qt::green));
        this->menu_panel_->user_msg_label_->
                setText("Trajectory remains feasible!");
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


    this->drone_traj3dof_data_.K = this->model_->fly_->P.K;
    for (quint32 k = 0; k < this->model_->fly_->P.K; k++) {
//  this->drone_traj3dof_data_.clock_angle(k) = 90.0/180.0*3.141592*P.dt*k;

        this->drone_traj3dof_data_.time(k) = k * this->model_->fly_->P.dt;
        this->drone_traj3dof_data_.pos_ned(0, k) =
                this->model_->fly_->O.r[1][k];
        this->drone_traj3dof_data_.pos_ned(1, k) =
                this->model_->fly_->O.r[2][k];
        this->drone_traj3dof_data_.pos_ned(2, k) =
                this->model_->fly_->O.r[0][k];

        this->drone_traj3dof_data_.vel_ned(0, k) =
                this->model_->fly_->O.v[1][k];
        this->drone_traj3dof_data_.vel_ned(1, k) =
                this->model_->fly_->O.v[2][k];
        this->drone_traj3dof_data_.vel_ned(2, k) =
                this->model_->fly_->O.v[0][k];

        this->drone_traj3dof_data_.accl_ned(0, k) =
                this->model_->fly_->O.a[1][k];
        this->drone_traj3dof_data_.accl_ned(1, k) =
                this->model_->fly_->O.a[2][k];
        this->drone_traj3dof_data_.accl_ned(2, k) =
                this->model_->fly_->O.a[0][k] - 9.81;
    }
//    this->drone_traj3dof_data_.
    // Set up next solution.
    // SKYEFLY //
    // TODO(mceowen): No namespace in algorithm.h to specify reset function
    reset(this->model_->fly_->P, this->model_->fly_->I, this->model_->fly_->O);
//    qInfo() << "Solver took " << this->timer_compute_.elapsed() << "ms";
//    this->solver_difficulty_ = this->timer_compute_.elapsed();

    emit startCompute();
}

bool Controller::isFrozen() {
    bool frozen = exec_once_
            && ((timer_exec_.elapsed() / 1000)
                <= (this->model_->finaltime_ * 1.2));
    if (frozen) {
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

    std::cout << "pos:" << this->drone_traj3dof_data_.pos_ned.transpose()
              << std::endl;
    std::cout << "vel:" << this->drone_traj3dof_data_.vel_ned.transpose()
              << std::endl;
    std::cout << "accl:" << this->drone_traj3dof_data_.accl_ned.transpose()
              << std::endl;

    emit trajectoryExecuted(&this->drone_traj3dof_data_);
}

bool Controller::simDrone(uint64_t tick) {
    if (tick >= (uint64_t)this->model_->trajectory_->length()) {
        return false;
    }
    QPointF *pos = this->model_->trajectory_->value(tick);
    this->model_->drone_->pos_->setX(pos->x());
    this->model_->drone_->pos_->setY(pos->y());
    this->canvas_->update();
    return true;
}

void Controller::setPorts() {
    this->port_dialog_->setModel(this->model_);
    this->port_dialog_->open();
}

void Controller::setCanvas(Canvas *canvas) {
    this->canvas_ = canvas;
}

void Controller::addPathPoint(QPointF *point) {
    this->model_->addPathPoint(point);
}

void Controller::clearPathPoints() {
    this->model_->clearPath();
}

// ============ NETWORK CONTROLS ============

void Controller::startSockets() {
    // close old sockets
    this->closeSockets();

    // create drone socket
    if (this->model_->drone_->port_ > 0) {
        this->drone_socket_ = new DroneSocket(this->model_->drone_);
        connect(this,
                SIGNAL(trajectoryExecuted(const autogen::packet::traj3dof*)),
                this->drone_socket_,
                SLOT(rx_trajectory(const autogen::packet::traj3dof*)));
        connect(this->drone_socket_, SIGNAL(refresh_graphics()),
                this->canvas_, SLOT(update()));
    }

    // create final pos socket
    if (this->model_->final_pos_->port_ > 0) {
        this->final_point_socket_ = new PointSocket(this->model_->final_pos_);
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

}  // namespace interface
