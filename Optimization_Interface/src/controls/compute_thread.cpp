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
        // Do not compute new trajectories if executing
        // sent trajectory
        if (this->model_->isLiveReference()) continue;

        // Parameters
        double r_i[3] = { 0 };
        double v_i[3] = { 0 };
        double a_i[3] = { 0 };
        double r_f[3] = { 0 };
        this->model_->loadInitialTelem(r_i, v_i, a_i);
        this->model_->loadFinalPos(r_f);

        double wp[3][skyenet::MAX_WAYPOINTS] = { 0 };
        this->model_->loadWaypoints(wp);

        skyenet::params P = this->model_->getSkyeFlyParams();

        // Initialize problem
        this->fly_->init_problem1(P, r_i, v_i, a_i, r_f, wp);

        // Run SCvx algorithm
        skyenet::outputs O = this->fly_->update();

        // Iterations in resulting trajectory
        quint32 size = P.K;
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
            drone_traj3dof_data.time(i) = i * P.dt;
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

        // Do not display new trajectories if executing
        // sent trajectory. Needed because sometimes compute
        // overlaps with setting live reference mode
        if (this->model_->isLiveReference()) continue;

        // set points on graphical display
        this->model_->setPathPoints(trajectory);
        this->model_->setCurrTraj3dof(drone_traj3dof_data);

        // OUTPUT VIOLATIONS: initial and final pos violation
        qreal accum = pow(O.r_f_relax[0], 2)
                + pow(O.r_f_relax[1], 2)
                + pow(O.r_f_relax[2], 2);

        if (accum > 0.25) {
            this->model_->setIsValidTraj(false);
            emit this->setPathColor(true);
            emit this->setMessage(FEEDBACK_CODE::GENERIC_INFEASIBLE);
        } else {
            this->model_->setIsValidTraj(true);
            emit this->setPathColor(false);
            emit this->setMessage(FEEDBACK_CODE::FEASIBLE);
        }
        emit updateGraphics();
    }
}

}  // namespace optgui
