// TITLE:   Optimization_Interface/src/controls/compute_thread.cpp
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

#include "include/globals.h"
#include "include/controls/compute_thread.h"

#include <algorithm>
#include <QDebug>

namespace optgui {

ComputeThread::ComputeThread(ConstraintModel *model) {
    this->model_ = model;
    this->run_loop_ = true;
    this->fly_ = new skyenet::SkyeFly();
}

ComputeThread::~ComputeThread() {
    delete this->fly_;
}

void ComputeThread::stopCompute() {
    this->run_loop_ = false;
}

void ComputeThread::run() {
    while (this->run_loop_) {
        // Parameters

        // Relaxation variables
        this->fly_->P.dK = 1;
        this->fly_->P.n_recalcs = 14;
        this->fly_->P.g[0] = -9.81;
        this->fly_->P.g[1] = 0.0;
        this->fly_->P.g[2] = 0.0;
        this->fly_->P.a_min = 5.0;   // Minimum accel.
        this->fly_->P.a_max = 12.0;  // Maximum accel.
        this->fly_->P.theta_max = 45.0 * skyenet::DEG2RAD;
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

        // Number of points on the trajectory (resolution)
        this->fly_->P.K = std::max(std::min(
                this->model_->getHorizon(), skyenet::MAX_HORIZON), 5u);
        // duration of flight
        this->fly_->P.tf = this->model_->getFinaltime();
        // 'resolution'
        this->fly_->P.dt = this->fly_->P.tf / (this->fly_->P.K - 1.0);
        // Circle constraints | H(r - p) |^2 > R^2 where p is the center of the
        // circle and R is the radius (H some linear transform)
        this->fly_->P.obs.n = this->model_->
                loadEllipseConstraints(this->fly_->P.obs.R,
                                       this->fly_->P.obs.c_e,
                                       this->fly_->P.obs.c_n);
        // Affine constraints Ax leq b
        this->fly_->P.cpos.n = this->model_->
                loadPosConstraints(this->fly_->P.cpos.A,
                                   this->fly_->P.cpos.b);

        // Inputs
        this->model_->loadInitialPos(this->fly_->I.r_i);
        this->fly_->I.v_i[0] =  0.0;
        this->fly_->I.v_i[1] =  0.0;  // this->model_->drone_->vel_->x();
        this->fly_->I.v_i[2] =  0.0;  // this->model_->drone_->vel_->x();
        this->fly_->I.a_i[0] = -this->fly_->P.g[0];
        this->fly_->I.a_i[1] = -this->fly_->P.g[1];
        this->fly_->I.a_i[2] = -this->fly_->P.g[2];
        this->model_->loadFinalPos(this->fly_->I.r_f);
        this->fly_->I.v_f[0] =  0.0;
        this->fly_->I.v_f[1] =  0.0;
        this->fly_->I.v_f[2] =  0.0;
        this->fly_->I.a_f[0] = -this->fly_->P.g[0];
        this->fly_->I.a_f[1] = -this->fly_->P.g[1];
        this->fly_->I.a_f[2] = -this->fly_->P.g[2];

        // TODO: waypoints
        if(this->model_->hasWaypoints()) {
            this->fly_->P.wprelax[0] = 10;
            this->fly_->P.wp_idx[0] = this->fly_->P.K/2;
            QPointF _wp = guiXyzToNED(this->model_->getWaypoint(0));
            this->fly_->I.wp[0][0] = 0;
            this->fly_->I.wp[0][1] = _wp.x();
            this->fly_->I.wp[0][2] = _wp.y();
        } else {
            this->fly_->P.wprelax[0] = 0;
        }



        // Initialize.
        this->fly_->init();

        // Run SCvx algorithm
        this->fly_->run();

        // Iterations in resulting trajectory
        quint32 size = this->fly_->P.K;
        // GUI trajecotry points
        QVector<QPointF> trajectory = QVector<QPointF>();
        // Mikipilot trajectory to send to drone
        autogen::packet::traj3dof drone_traj3dof_data;
        drone_traj3dof_data.K = size;

        for (quint32 i = 0; i < size; i++) {
            // Add points to GUI trajectory
            QPointF gui_coords = nedToGuiXyz(this->fly_->O.r[1][i],
                                             this->fly_->O.r[2][i]);
            trajectory.append(gui_coords);

            // Add data to mikipilot trajectory
            // drone_traj3dof_data.clock_angle(k) = 90.0/180.0*3.141592*P.dt*k;
            drone_traj3dof_data.time(i) = i * this->fly_->P.dt;
            drone_traj3dof_data.pos_ned(0, i) =
                    this->fly_->O.r[1][i];
            drone_traj3dof_data.pos_ned(1, i) =
                    this->fly_->O.r[2][i];
            drone_traj3dof_data.pos_ned(2, i) =
                    this->fly_->O.r[0][i];

            drone_traj3dof_data.vel_ned(0, i) =
                    this->fly_->O.v[1][i];
            drone_traj3dof_data.vel_ned(1, i) =
                    this->fly_->O.v[2][i];
            drone_traj3dof_data.vel_ned(2, i) =
                    this->fly_->O.v[0][i];

            drone_traj3dof_data.accl_ned(0, i) =
                    this->fly_->O.a[1][i];
            drone_traj3dof_data.accl_ned(1, i) =
                    this->fly_->O.a[2][i];
            drone_traj3dof_data.accl_ned(2, i) =
                    this->fly_->O.a[0][i] - 9.81;
        }
        // set points on graphical display
        this->model_->setPathPoints(trajectory);
        this->model_->setPathSentPoints(trajectory);
        this->model_->setTraj3dof(drone_traj3dof_data);

        // OUTPUT VIOLATIONS: initial and final pos violation
        qreal accum = pow(this->fly_->O.r_f_relax[0], 2)
                + pow(this->fly_->O.r_f_relax[1], 2)
                + pow(this->fly_->O.r_f_relax[2], 2);

        // TODO(dtsull16): add signals to update these in controller
        if (accum > 0.25) {
            this->model_->setIsValidTraj(false);
            emit this->setPathColor(true);
            emit this->setMessage("Increase final time to regain feasibility!");
        } else {
            this->model_->setIsValidTraj(true);
            emit this->setPathColor(false);
            emit this->setMessage("Trajectory remains feasible!");
        }
        emit updateGraphics();

        // Debugging outputs
        /*
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

        // TODO(mceowen): No namespace in algorithm.h to specify reset function
        reset(this->fly_->P, this->fly_->I, this->fly_->O);
    }
}

}  // namespace optgui
