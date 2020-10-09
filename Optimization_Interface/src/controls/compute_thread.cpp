// TITLE:   Optimization_Interface/src/controls/compute_thread.cpp
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

#include "include/controls/compute_thread.h"
#include "include/graphics/path_graphics_item.h"

#include <algorithm>
#include <QVector3D>

namespace optgui {

ComputeThread::ComputeThread(ConstraintModel *model,
                             DroneGraphicsItem *drone,
                             PathGraphicsItem *traj_graphic) : mutex_() {
    this->model_ = model;
    // start running compute loop on construction
    this->run_loop_ = true;
    this->drone_ = drone;
    this->traj_graphic_ = traj_graphic;
    this->target_ = nullptr;
    this->target_changed_ = true;
}

ComputeThread::~ComputeThread() {
    QMutexLocker(&this->mutex_);
}

void ComputeThread::stopCompute() {
    // flag loop to stop after the next iteration
    QMutexLocker(&this->mutex_);
    this->run_loop_ = false;
}

void ComputeThread::setTarget(PointModelItem *target) {
    QMutexLocker(&this->mutex_);
    if (target != this->target_) {
        this->target_changed_ = true;
    }
    this->target_ = target;
}

void ComputeThread::reInit() {
    QMutexLocker(&this->mutex_);
    this->target_changed_ = true;
}

PointModelItem *ComputeThread::getTarget() {
    QMutexLocker(&this->mutex_);
    return this->target_;
}

bool ComputeThread::getRunFlag() {
    QMutexLocker(&this->mutex_);
    return this->run_loop_;
}

PathGraphicsItem *ComputeThread::getTrajGraphic() {
    QMutexLocker(&this->mutex_);
    return this->traj_graphic_;
}

DroneGraphicsItem *ComputeThread::getDroneGraphic() {
    QMutexLocker(&this->mutex_);
    return this->drone_;
}

void ComputeThread::run() {
    // run compute loop until flagged to stop
    while (this->getRunFlag()) {
        // Do not compute new trajectories if executing
        // sent trajectory
        if (this->model_->isLiveReference()) {
            continue;
        }

        // Do not compute trajectory if no final point selected
        if (this->getTarget() == nullptr) {
            // clear current trajectory
            this->getTrajGraphic()->model_->setPoints(QVector<QPointF>());
            autogen::packet::traj3dof empty_traj;
            this->model_->setCurrTraj3dof(this->drone_->model_, empty_traj);
            continue;
        }

        // Validate inputs
        QVector3D initial_pos = this->drone_->model_->getPos();
        QVector3D initial_vel = this->drone_->model_->getVel();
        QVector3D initial_acc = this->drone_->model_->getAccel();

        QPointF final_pos_2D = this->getTarget()->getPos();
        QVector3D final_pos = QVector3D(final_pos_2D.x(), final_pos_2D.y(), 0);
        QVector<QRegion> ellipse_regions = this->model_->getEllipseRegions();

        // validate inputs
        INPUT_CODE input_code = this->validateInputs(ellipse_regions,
                                                     initial_pos, final_pos);
        // set valid input and update message if changed
        if (this->model_->setIsValidInput(input_code)) {
            this->model_->updateEllipseColors();
            emit updateMessage(this->drone_->model_);
        }
        // Dont compute if invalid input
//        if (input_code != INPUT_CODE::VALID_INPUT) {
//            continue;
//        }

        // Parameters

        // Get params
        skyenet::params P = this->model_->getSkyeFlyParams();
        this->model_->loadEllipseConstraints(&P);
        this->model_->loadPosConstraints(&P);

        double r_i[3] = { 0 };
        double v_i[3] = { 0 };
        double a_i[3] = { 0 };
        double r_f[3] = { 0 };
        double wp[skyenet::MAX_WAYPOINTS][3] = {{ 0 }};

        // set initial drone pos
        QVector3D xyz_drone_pos = guiXyzToXyz(initial_pos);
        r_i[0] = xyz_drone_pos.x();
        r_i[1] = xyz_drone_pos.y();
        r_i[2] = xyz_drone_pos.z();

        // set initial drone vel
        QVector3D xyz_drone_vel = guiXyzToXyz(initial_vel);
        v_i[0] = xyz_drone_vel.x();
        v_i[1] = xyz_drone_vel.y();
        v_i[2] = xyz_drone_vel.z();

        // set iniital drone accel
        QVector3D xyz_drone_acc = guiXyzToXyz(initial_acc);
        a_i[0] = xyz_drone_acc.x();
        a_i[1] = xyz_drone_acc.y();
        a_i[2] = xyz_drone_acc.z();

        // set final pos
        QVector3D xyz_final_pos = guiXyzToXyz(final_pos);
        r_f[0] = xyz_final_pos.x();
        r_f[1] = xyz_final_pos.y();
        r_f[2] = xyz_final_pos.z();

        // set waypoints
        this->model_->loadWaypointConstraints(&P, wp);

        // Initialize problem
        this->fly_.setParams(P, r_i, v_i, a_i, r_f, wp);

        // check to reset inputs
        if (this->target_changed_) {
            this->target_changed_ = false;
            this->fly_.resetInputs(r_i, v_i, a_i, r_f, wp);
        }

        // Run SCvx algorithm for free or fixed final time
        skyenet::outputs const &O =
                this->fly_.update(this->model_->isFreeFinalTime());

        // Iterations in resulting trajectory
        quint32 size = P.K;
        // GUI trajecotry points
        QVector<QPointF> trajectory = QVector<QPointF>();
        // Mikipilot trajectory to send to drone
        autogen::packet::traj3dof drone_traj3dof_data;
        drone_traj3dof_data.K = size;

        for (quint32 i = 0; i < size; i++) {
            // Add points to GUI trajectory
            QVector3D gui_coords = xyzToGuiXyz(O.r[0][i],
                                               O.r[1][i],
                                               O.r[2][i]);
            trajectory.append(QPointF(gui_coords.x(),
                                      gui_coords.y()));

            // Add data to mikipilot trajectory
            // drone_traj3dof_data.clock_angle(k) = 90.0/180.0*3.141592*P.dt*k;
            drone_traj3dof_data.time(i) = O.t[i];

            // XYZ to NED conversion
            drone_traj3dof_data.pos_ned(0, i) =  O.r[1][i];
            drone_traj3dof_data.pos_ned(1, i) =  O.r[0][i];
            drone_traj3dof_data.pos_ned(2, i) = -O.r[2][i];

            drone_traj3dof_data.vel_ned(0, i) =  O.v[1][i];
            drone_traj3dof_data.vel_ned(1, i) =  O.v[0][i];
            drone_traj3dof_data.vel_ned(2, i) = -O.v[2][i];

            drone_traj3dof_data.accl_ned(0, i) =  O.a[1][i];
            drone_traj3dof_data.accl_ned(1, i) =  O.a[0][i];
            drone_traj3dof_data.accl_ned(2, i) = -O.a[2][i];
        }

        // Do not display new trajectories if executing
        // sent trajectory. Needed because sometimes compute
        // overlaps with setting live reference mode
        if (this->model_->isLiveReference() || !this->getRunFlag()) continue;

        // set points on graphical display
        this->getTrajGraphic()->model_->setPoints(trajectory);
        this->model_->setCurrTraj3dof(this->drone_->model_,
                                      drone_traj3dof_data);

        // OUTPUT VIOLATIONS: initial and final pos violation
        qreal accum = pow(O.rf_relax[0], 2)  // final pos
                    + pow(O.rf_relax[1], 2)
                    + pow(O.rf_relax[2], 2)

                    + pow(O.ri_relax[0], 2)  // initial pos
                    + pow(O.ri_relax[1], 2)
                    + pow(O.ri_relax[2], 2)

                    + pow(O.dtau, 2);  // change in time

        bool is_feasible;
        if (accum > 0.25) {
            // infeasible traj, set feasibility code and traj color to red
            this->model_->setIsValidTraj(FEASIBILITY_CODE::INFEASIBLE);
            is_feasible = false;
        } else {
            // feasible traj, set feasibility code and traj color to nominal
            this->model_->setIsValidTraj(FEASIBILITY_CODE::FEASIBLE);
            is_feasible = true;
        }
        if (this->model_->isFreeFinalTime()) {
            emit finalTime(this->drone_->model_, O.t[size - 1]);
        }
        emit updateMessage(this->drone_->model_);

        this->setFeasibilityColor(is_feasible);
    }
}

void ComputeThread::setFeasibilityColor(bool is_feasible) {
    // get graphics items
    DroneGraphicsItem *drone = this->getDroneGraphic();
    PathGraphicsItem *traj = this->getTrajGraphic();

    // set feasiblility color
    if (is_feasible) {
        traj->setColor(YELLOW);
        drone->setIsFeasible(true);
    } else {
        traj->setColor(RED);
        drone->setIsFeasible(false);
    }
    emit updateGraphics(traj, drone);
}

INPUT_CODE ComputeThread::validateInputs(
        QVector<QRegion> const &ellipse_regions,
        QVector3D const &initial_pos,
        QVector3D const &final_pos) {
    // get truncated drone pos
    QPoint trunc_intial_pos(initial_pos.x(), initial_pos.y());
    // get truncated final pos
    QPoint trunc_final_pos(final_pos.x(), final_pos.y());

    // Note: Only can use QPoint. coords are still in pixels so QPoint instead
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
        /* Temp remove for performance
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
        */
    }
    return INPUT_CODE::VALID_INPUT;
}

}  // namespace optgui
