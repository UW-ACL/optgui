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
        if (this->model_->isLiveReference()) {
            continue;
        }

        // Do not compute trajectory if no final point selected
        if (!this->model_->hasCurrFinalPoint()) {
            // clear current trajectory
            this->model_->setPathPoints(QVector<QPointF>());
            autogen::packet::traj3dof empty_traj;
            this->model_->setCurrTraj3dof(empty_traj);
            continue;
        }

        // Validate inputs
        QPointF initial_pos = this->model_->getInitialPos();
        QPointF final_pos = this->model_->getFinalPos();
        QVector<QRegion> ellipse_regions = this->model_->getEllipseRegions();
        INPUT_CODE input_code = this->validateInputs(ellipse_regions,
                                                     initial_pos, final_pos);
        // set valid input and update message if changed
        if (this->model_->setIsValidInput(input_code)) {
            this->model_->updateEllipseColors();
            emit updateMessage();
        }
        // Dont compute if invalid input
        if (input_code != INPUT_CODE::VALID_INPUT) {
            continue;
        }

        // Input is valid, get additional inputs
        QPointF initial_vel = this->model_->getInitialVel();
        QPointF initial_acc = this->model_->getInitialAcc();

        // Parameters

        // Get params
        skyenet::params P = this->model_->getSkyeFlyParams();
        this->model_->loadEllipseConstraints(&P);
        this->model_->loadPosConstraints(&P);

        double r_i[3] = { 0 };
        double v_i[3] = { 0 };
        double a_i[3] = { 0 };
        double r_f[3] = { 0 };
        double wp[3][skyenet::MAX_WAYPOINTS] = { 0 };

        // set initial drone pos
        QPointF ned_drone_pos = guiXyzToNED(initial_pos.x(), initial_pos.y());
        r_i[1] = ned_drone_pos.x();
        r_i[2] = ned_drone_pos.y();

        // set initial drone vel
        QPointF ned_drone_vel = guiXyzToNED(initial_vel.x(), initial_vel.y());
        v_i[1] = ned_drone_vel.x();
        v_i[2] = ned_drone_vel.y();

        // set iniital drone accel
        QPointF ned_drone_acc = guiXyzToNED(initial_acc.x(), initial_acc.y());
        a_i[1] = ned_drone_acc.x();
        a_i[2] = ned_drone_acc.y();

        // set final pos
        QPointF ned_final_pos = guiXyzToNED(final_pos.x(), final_pos.y());
        r_f[1] = ned_final_pos.x();
        r_f[2] = ned_final_pos.y();

        // set waypoints
        this->model_->loadWaypointConstraints(&P, wp);

        // Initialize problem
        this->fly_->setParams(P, r_i, v_i, a_i, r_f, wp);

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
            this->model_->setIsValidTraj(FEASIBILITY_CODE::INFEASIBLE);
            emit this->setPathColor(true);
        } else {
            this->model_->setIsValidTraj(FEASIBILITY_CODE::FEASIBLE);
            emit this->setPathColor(false);
        }
        emit updateMessage();
        emit updateGraphics();
    }
}

INPUT_CODE ComputeThread::validateInputs(
        QVector<QRegion> const &ellipse_regions,
        QPointF const &initial_pos,
        QPointF const &final_pos) {
    // get truncated drone pos
    QPoint trunc_intial_pos(initial_pos.x(), initial_pos.y());
    // get truncated final pos
    QPoint trunc_final_pos(final_pos.x(), final_pos.y());

    // Note: coords are still in cm so QPoint instead
    // of QPointF should be fine

    for (int i = 0; i < ellipse_regions.size(); i++) {
        QRegion const &region = ellipse_regions.at(i);

        // check if contains drone
        if (region.boundingRect().contains(trunc_intial_pos)) {
            // check cheaply in rect, then in ellipse
            if (region.contains(trunc_intial_pos)) {
                return INPUT_CODE::DRONE_OVERLAP;
            }
        }

        // check if contains final point
        if (region.boundingRect().contains(trunc_final_pos)) {
            // check cheaply in rect, then in ellipse
            if (region.contains(trunc_final_pos)) {
                return INPUT_CODE::FINAL_POS_OVERLAP;
            }
        }

        // check if overlapping with any other ellipses
        for (int j = i+1; j < ellipse_regions.size(); j++) {
            // First look at intersection of boundingRects (cheaper to do)
            if (region.boundingRect().intersects(
                        ellipse_regions.at(j).boundingRect())) {
                // If bounding rect overlaps, then check fine intersection
                if (region.intersects(ellipse_regions.at(j))) {
                    return INPUT_CODE::OBS_OVERLAP;
                }
            }
        }
    }
    return INPUT_CODE::VALID_INPUT;
}

}  // namespace optgui
