// TITLE:   Optimization_Interface/src/controls/compute_thread.cpp
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

#include "include/controls/compute_thread.h"

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

#include "include/graphics/point_graphics_item.h"
#include "include/graphics/ellipse_graphics_item.h"
#include "include/graphics/polygon_graphics_item.h"
#include "include/graphics/plane_graphics_item.h"
#include "include/globals.h"
#include "include/window/menu_panel.h"

namespace interface {

ComputeThread::ComputeThread(ConstraintModel *model) {
    this->model_ = model;
    this->initializeFly();

    connect(this, SIGNAL(startCompute()), this, SLOT(compute()));
    connect(this, SIGNAL(updateGraphics()),
            this->canvas_, SLOT(updatePathGraphicsItem(this->canvas_->path_graphic_)));
}

ComputeThread::~ComputeThread() {
    delete this->fly_;
}

void ComputeThread::initializeFly() {
    this->fly_ = new SkyeFly();

    // Relaxation variables
    this->fly_->P.dK = 1;
    this->fly_->P.n_recalcs = 14;
    this->fly_->P.g[0] = -9.81;
    this->fly_->P.g[1] = 0.0;
    this->fly_->P.g[2] = 0.0;
    this->fly_->P.a_min = 5.0;   // Minimum accel.
    this->fly_->P.a_max = 12.0;  // Maximum accel.
    this->fly_->P.theta_max = 45.0 * DEG2RAD;
    this->fly_->P.q_max = 0.0;

    this->fly_->P.max_iter = 10;
    this->fly_->P.Delta_i = 100.0;
    this->fly_->P.lambda = 1e2;
    this->fly_->P.alpha = 2.0;
    this->fly_->P.dL_tol = 1e-1;
    this->fly_->P.rho_0 = -1e-1;
    this->fly_->P.rho_1 = 0.25;
    this->fly_->P.rho_2 = 0.90;
    this->fly_->P.rirelax = 1000;
    this->fly_->P.rfrelax = 10;
}

void ComputeThread::run() {
    emit this->startCompute();
}

void ComputeThread::compute() {
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
            loadEllipse(this->model_->fly_->P.obs.R,
                        this->model_->fly_->P.obs.c_e,
                        this->model_->fly_->P.obs.c_n);
    // Affine constraints Ax \leq b
    this->model_->fly_->P.cpos.n = model_->
            loadPosConstraint(this->model_->fly_->P.cpos.A,
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
    this->model_->clearPath();
//    this->model_->trajectory_->clear();

    for (uint32_t i = 0; i < this->model_->fly_->P.K; i++) {
        // TODO: Look into memory leak
        this->model_->addPathPoint(new QPointF(this->model_->fly_->O.r[2][i] * GRID_SIZE,
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


    this->model_->drone_traj3dof_data_.K = this->model_->fly_->P.K;
    for (quint32 k = 0; k < this->model_->fly_->P.K; k++) {
//  this->drone_traj3dof_data_.clock_angle(k) = 90.0/180.0*3.141592*P.dt*k;

        this->model_->drone_traj3dof_data_.time(k) = k * this->model_->fly_->P.dt;
        this->model_->drone_traj3dof_data_.pos_ned(0, k) =
                this->model_->fly_->O.r[1][k];
        this->model_->drone_traj3dof_data_.pos_ned(1, k) =
                this->model_->fly_->O.r[2][k];
        this->model_->drone_traj3dof_data_.pos_ned(2, k) =
                this->model_->fly_->O.r[0][k];

        this->model_->drone_traj3dof_data_.vel_ned(0, k) =
                this->model_->fly_->O.v[1][k];
        this->model_->drone_traj3dof_data_.vel_ned(1, k) =
                this->model_->fly_->O.v[2][k];
        this->model_->drone_traj3dof_data_.vel_ned(2, k) =
                this->model_->fly_->O.v[0][k];

        this->model_->drone_traj3dof_data_.accl_ned(0, k) =
                this->model_->fly_->O.a[1][k];
        this->model_->drone_traj3dof_data_.accl_ned(1, k) =
                this->model_->fly_->O.a[2][k];
        this->model_->drone_traj3dof_data_.accl_ned(2, k) =
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

}  // namespace interface
