// TITLE:   Optimization_Interface/src/controls/compute_thread.cpp
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

#include "include/controls/compute_thread.h"

#include <algorithm>

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

        /*
         *      fly_->init_problem1(this->model_->getHorizon(),
         *                          this->model_->getFinalTime(),
         *                          this->model_->ellipses_->size(),
         *                          this->model_->planes_->size(),
         *                          this->model_->polygons_->size(),
         *                         );
         *      fly_->update(this->model_->drone_->pos_,
         *                   this->model_->final_pos_->getPos())
         *                   this->model->getFinalTime();
         *
         *      fly_->output(this->model_->drone_traj3dof_data_);
         *
         */

        //TODO(mceowen):Set fully on skyenet side while maintaining thread locking
        //Initialize problem 1
        this->fly_->init_problem1(this->model_->getHorizon(),   // Number of points on the trajectory (resolution)
                                  this->model_->getFinaltime(), // Duration of flight [s]
                                  this->model_->                // Circle constraints | H(r - p) |^2 > R^2
                                      loadEllipseConstraints(this->fly_->P.obs.R,
                                                             this->fly_->P.obs.c_e,
                                                             this->fly_->P.obs.c_n),
                                  this->model_->                // Affine constraints Ax leq b
                                      loadPosConstraints(this->fly_->P.cpos.A,
                                                         this->fly_->P.cpos.b),
                                  this->model_->
                                    loadInitialPos(this->fly_->I.r_i),
                                  this->model_->
                                    loadFinalPos(this->fly_->I.r_f)
                                  );


        // Run SCvx algorithm
        this->fly_->run(); //this->fly_->update();

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
        skyenet::reset(this->fly_->P, this->fly_->I, this->fly_->O);
    }
}

}  // namespace optgui
