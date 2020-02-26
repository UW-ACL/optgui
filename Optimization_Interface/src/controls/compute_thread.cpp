// TITLE:   Optimization_Interface/src/controls/compute_thread.cpp
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

#include "include/controls/compute_thread.h"

#include <algorithm>

#include <QDebug>

namespace optgui {

ComputeThread::ComputeThread(ConstraintModel *model) {
    this->model_ = model;
    this->run_loop_ = true;
    this->fly_ = new skyenet::SkyeFly();
    this->P = skyenet::getDefaultP();
}

ComputeThread::~ComputeThread() {
    delete this->fly_;
    //delete this->P;
}

void ComputeThread::stopCompute() {
    this->run_loop_ = false;
}

void ComputeThread::run() {
    while (this->run_loop_) {
        // Parameters   
        double r_i[3] = { 0 };
        double r_f[3] = { 0 };
        this->model_->loadInitialPos(r_i);
        this->model_->loadFinalPos(r_f);
        // Horizon
        this->P.K = std::max(std::min(
                this->model_->getHorizon(), skyenet::MAX_HORIZON), 5u);
        // duration of flight
        this->P.tf = this->model_->getFinaltime();
        // 'resolution'
        // Circle constraints | H(r - p) |^2 > R^2 where p is the center of the
        // circle and R is the radius (H some linear transform)
        this->model_->loadEllipseConstraints(this->P);
        // Affine constraints Ax leq b
        this->model_->loadPosConstraints(this->P);

        this->fly_->init_problem1(P, r_i, r_f);

        // Run SCvx algorithm
        skyenet::outputs O = this->fly_->update();
        // qDebug() << O.r[1][0];

        // Iterations in resulting trajectory
        quint32 size = this->P.K;
        // GUI trajecotry points
        QVector<QPointF> trajectory = QVector<QPointF>();
        // Mikipilot trajectory to send to drone
        autogen::packet::traj3dof drone_traj3dof_data;
        drone_traj3dof_data.K = size;

        for (quint32 i = 0; i < size; i++) {
            // Add points to GUI trajectory
            QPointF gui_coords = nedToGuiXyz(O.r[1][i],
                                             O.r[2][i]);
            trajectory.append(gui_coords);

            // Add data to mikipilot trajectory
            // drone_traj3dof_data.clock_angle(k) = 90.0/180.0*3.141592*P.dt*k;
            drone_traj3dof_data.time(i) = i * this->P.dt;
            drone_traj3dof_data.pos_ned(0, i) = O.r[1][i];
            drone_traj3dof_data.pos_ned(1, i) = O.r[2][i];
            drone_traj3dof_data.pos_ned(2, i) = O.r[0][i];

            drone_traj3dof_data.vel_ned(0, i) = O.v[1][i];
            drone_traj3dof_data.vel_ned(1, i) = O.v[2][i];
            drone_traj3dof_data.vel_ned(2, i) = O.v[0][i];

            drone_traj3dof_data.accl_ned(0, i) = O.a[1][i];
            drone_traj3dof_data.accl_ned(1, i) = O.a[2][i];
            drone_traj3dof_data.accl_ned(2, i) = O.a[0][i] - 9.81;
        }
        // set points on graphical display
        this->model_->setPathPoints(trajectory);
        this->model_->setTraj3dof(drone_traj3dof_data);

        // OUTPUT VIOLATIONS: initial and final pos violation
        qreal accum = pow(O.r_f_relax[0], 2)
                + pow(O.r_f_relax[1], 2)
                + pow(O.r_f_relax[2], 2);

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
    }
}

}  // namespace optgui
