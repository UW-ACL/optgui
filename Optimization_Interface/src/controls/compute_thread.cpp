// TITLE:   Optimization_Interface/src/controls/compute_thread.cpp
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

#include "include/controls/compute_thread.h"
#include "include/graphics/path_graphics_item.h"

#include <algorithm>
#include <QVector3D>
#include <iostream>

namespace optgui {

ComputeThread::ComputeThread(ConstraintModel *model,
                             DroneGraphicsItem *drone,
                             PathGraphicsItem *traj_graphic_sol,
                             PathGraphicsItem *traj_graphic_sim,
                             PathGraphicsItem *traj_graphic_sol_pool[skyenet::MAX_TARGETS],
                             PathGraphicsItem *traj_graphic_sim_pool[skyenet::MAX_TARGETS]) : mutex_() {
    this->model_ = model;

    // start running compute loop on construction
    this->run_loop_ = true;
    this->drone_ = drone;
    this->traj_graphic_sol_ = traj_graphic_sol;
    this->traj_graphic_sim_ = traj_graphic_sim;
    this->target_ = nullptr;
    this->target_changed_ = true;
    this->num_pooled_targets_ = 0;
    this->julia_initialized_ = false;
    for (int i = 0; i < skyenet::MAX_TARGETS; i++) {
        this->traj_graphic_sol_pool_[i] = traj_graphic_sol_pool[i];
        this->traj_graphic_sim_pool_[i] = traj_graphic_sim_pool[i];
    }

    // Set drawing flags for PathGraphicsItems
    traj_graphic_sol_->draw_lines = false;
    traj_graphic_sim_->draw_points = false;
    for (int i = 0; i < skyenet::MAX_TARGETS; i++) {
      traj_graphic_sol_pool_[i]->draw_lines = false;
      traj_graphic_sim_pool_[i]->draw_points = false;
    }
}

ComputeThread::~ComputeThread() {
    QMutexLocker(&this->mutex_);
}

void ComputeThread::stopCompute() {
    // flag loop to stop after the next iteration
    QMutexLocker(&this->mutex_);
    this->run_loop_ = false;
}

void ComputeThread::initializeJulia() {
    QMutexLocker(&this->mutex_);
    if (!this->julia_initialized_) {
        int argc_julia;
        char *argv_julia;
        skyenet::initializeJulia(argc_julia, &argv_julia);
        this->julia_initialized_ = true;
    }
}

void ComputeThread::setTarget(PointModelItem *target) {
    QMutexLocker(&this->mutex_);
    if (target != this->target_) {
        this->target_changed_ = true;
    }
    this->target_ = target;
}

void ComputeThread::setPooledTargets(PointModelItem *targets[skyenet::MAX_TARGETS], quint32 num_targs) {
    QMutexLocker(&this->mutex_);
    for (int i = 0; i < skyenet::MAX_TARGETS; i++) {
        this->target_pool_[i] = targets[i];
    }
    this->num_pooled_targets_ = num_targs;
}

// void ComputeThread::addPooledTarget(PointModelItem *target) {
//     QMutexLocker(&this->mutex_);
//     // Only add if a new target to avoid duplication
//     // Bandaid: this duplication will happen by default w/ any new selections or object additions in GUI.
//     bool dupe = false;
//     for (int i = 0; i < this->num_pooled_targets_; i++) {
//         if (target == this->target_pool_[i]) {
//             dupe = true;
//         }
//     }
//     if (dupe == false){
//         // Append target to pool
//         this->target_pool_[this->num_pooled_targets_] = target;
//         this->num_pooled_targets_++;
//         this->target_changed_ = true;
//     }
// }

// void ComputeThread::checkRemovedTarget() {
//     QMutexLocker(&this->mutex_);
//     QList<QGraphicsItem *> all_items = this->scene()->items();
//     PointGraphicsItem* item;
//     bool target_found = false;
//     for (int i = this->num_pooled_targets_-1; i >= 0; i--) {
//         for (QGraphicsItem * item : all_items) {
//             if (item->type() == GRAPHICS_TYPE::POINT_GRAPHIC) {
//                 item = qgraphicsitem_cast<PointGraphicsItem *>(item)->model_;
//                 if (item == this->target_pool_[i]) {
//                     target_found = true;
//                     continue;
//                 }
//             }
//         }
//         if (!target_found) {
//             for (int j = i; j < skyenet::MAX_TARGETS-1; j++){
//                 this->target_pool_[j] = this->target_pool_[j+1];
//             }
//             this->num_pooled_targets_--;
//         }
//     }
// }

void ComputeThread::reInit() {
    QMutexLocker(&this->mutex_);
    this->target_changed_ = true;
}

PointModelItem *ComputeThread::getTarget() {
    QMutexLocker(&this->mutex_);
    return this->target_;
}

PointModelItem *ComputeThread::getPooledTarget(quint32 tag) {
    QMutexLocker(&this->mutex_);
    return this->target_pool_[tag];
}

bool ComputeThread::getRunFlag() {
    QMutexLocker(&this->mutex_);
    return this->run_loop_;
}

PathGraphicsItem *ComputeThread::getTrajSolGraphic() {
    QMutexLocker(&this->mutex_);
    return this->traj_graphic_sol_;
}

PathGraphicsItem *ComputeThread::getTrajSimGraphic() {
    QMutexLocker(&this->mutex_);
    return this->traj_graphic_sim_;
}

PathGraphicsItem *ComputeThread::getPooledTrajSolGraphic(quint32 tag) {
    QMutexLocker(&this->mutex_);
    return this->traj_graphic_sol_pool_[tag];
}

PathGraphicsItem *ComputeThread::getPooledTrajSimGraphic(quint32 tag) {
    QMutexLocker(&this->mutex_);
    return this->traj_graphic_sim_pool_[tag];
}

DroneGraphicsItem *ComputeThread::getDroneGraphic() {
    QMutexLocker(&this->mutex_);
    return this->drone_;
}

void ComputeThread::run() {
    // run compute loop until flagged to stop
    while (this->getRunFlag()) {

        // Initialize Julia
        this->initializeJulia();

        // ..:: Trajectory computation decision ::..
        // Do not compute new trajectories if executing
        // sent trajectory
        if (this->model_->isLiveReference()) {
            continue;
        }

        // Do not compute trajectory if no final point selected
        if (this->getTarget() == nullptr) {
            // clear current trajectory
            this->getTrajSolGraphic()->model_->setPoints(QVector<QPointF>());
            this->getTrajSimGraphic()->model_->setPoints(QVector<QPointF>());
            autogen::packet::traj2dof empty_traj;
            this->model_->setCurrTraj2dof(this->drone_->model_, empty_traj);
            continue;
        }

        // Remove any targets that are no longer valid
        // this->checkRemovedTargets();

        // ..:: Validate inputs ::..
        // Find initial state, selected target state and ellipse regions
        QVector3D initial_pos = this->drone_->model_->getPos();
        QVector3D initial_vel = this->drone_->model_->getVel();
        QVector3D initial_acc = this->drone_->model_->getAccel();
        QVector<QRegion> ellipse_regions = this->model_->getEllipseRegions();
        QPointF sel_final_pos_2D = this->getTarget()->getPos();
        QVector3D sel_final_pos = QVector3D(sel_final_pos_2D.x(), sel_final_pos_2D.y(), 0);

        // Loop through all pooled targets for validation
        for (int i = 0; i < this->num_pooled_targets_; i++) {
            // Grab the currently-considered target in the pool
            QPointF curr_final_pos_2D = this->getPooledTarget(i)->getPos();
            QVector3D curr_final_pos = QVector3D(curr_final_pos_2D.x(), curr_final_pos_2D.y(), 0);

            // validate inputs
            INPUT_CODE input_code = this->validateInputs(ellipse_regions,
                                                        initial_pos, curr_final_pos);
            // set valid input and update message if changed
            if (this->model_->setIsValidInput(input_code)) {
                this->model_->updateEllipseColors();
                emit updateMessage(this->drone_->model_);
            }
            // Dont compute if invalid input
            //        if (input_code != INPUT_CODE::VALID_INPUT) {
            //            continue;
            //        }
        }

        // ..:: Build Skyenet parameters ::..

        // Get params
        skyenet::params P = this->model_->getSkyeFlyParams();
        this->model_->loadEllipseConstraints(&P);
        this->model_->loadCylinderConstraints(&P);
        this->model_->loadPosConstraints(&P);

        double r_i[3] = { 0 };
        double v_i[3] = { 0 };
        double a_i[3] = { 0 };
        double r_f[3][skyenet::MAX_TARGETS] = {{ 0 }};
        double wp[skyenet::MAX_WAYPOINTS][3] = {{ 0 }};

        // set initial drone pos
        QVector3D xyz_drone_pos = guiXyzToXyz(initial_pos);
        r_i[0] = xyz_drone_pos.x();
        r_i[1] = xyz_drone_pos.y();
        r_i[2] = 0.0;

        // set initial drone vel
        QVector3D xyz_drone_vel = guiXyzToXyz(initial_vel);
        v_i[0] = xyz_drone_vel.x();
        v_i[1] = xyz_drone_vel.y();
        v_i[2] = xyz_drone_vel.z();

        // set initial drone accel
        QVector3D xyz_drone_acc = guiXyzToXyz(initial_acc);
        a_i[0] = xyz_drone_acc.x();
        a_i[1] = xyz_drone_acc.y();
        a_i[2] = xyz_drone_acc.z();

        // set final positions
        int sel_target_tag;
        for (int i = 0; i < this->num_pooled_targets_; i++) {
            QPointF curr_final_pos_2D = this->getPooledTarget(i)->getPos();
            QVector3D curr_final_pos = QVector3D(curr_final_pos_2D.x(), curr_final_pos_2D.y(), 0);
            QVector3D xyz_curr_final_pos = guiXyzToXyz(curr_final_pos);
            r_f[0][i] = xyz_curr_final_pos.x();
            r_f[1][i] = xyz_curr_final_pos.y();
            r_f[2][i] = xyz_curr_final_pos.z();

            // Check to see if this target in the pool is the selected target
            // if ((curr_final_pos - sel_final_pos).length() < 1e-6) {
            if (this->getPooledTarget(i) == this->getTarget()) {
                sel_target_tag = i;
            }
        }

        // set waypoints
        this->model_->loadWaypointConstraints(&P, wp);

        // Initialize problem
        this->fly_.setParams(P, r_i, v_i, a_i, r_f, wp);

        // ..:: Run SCvx algorithm for fixed final time ::..
        skyenet::outputs const &O = this->fly_.update(this->num_pooled_targets_);

        // Iterations in resulting trajectory
        quint32 size_sol = P.K;
        quint32 size_sim = P.sim_steps*(P.K-1);
        
        // ..:: Build Mikipilot trajectory to send to drone ::..
        autogen::packet::traj2dof drone_traj2dof_data;
        quint32 max_packet_nodes = MIN(size_sim, drone_traj2dof_data.time.length());
        quint32 sim_idx;
        
        drone_traj2dof_data.K = max_packet_nodes;
        for (quint32 i = 0; i < max_packet_nodes; i++){
            if (max_packet_nodes <= drone_traj2dof_data.time.length()){
                sim_idx = i;
            } else { // must perform downsampling
                sim_idx = quint32(round(float(i)*float(size_sim)/float(drone_traj2dof_data.time.length())));
            }

            // Add data to mikipilot trajectory
            drone_traj2dof_data.time(i) = O.t_sim[sim_idx][sel_target_tag];

            // XYZ to NED conversion
            drone_traj2dof_data.pos_ned(0, i)  =  O.r_sim[1][sim_idx][sel_target_tag];
            drone_traj2dof_data.pos_ned(1, i)  =  O.r_sim[0][sim_idx][sel_target_tag];
            drone_traj2dof_data.vel_ned(0, i)  =  O.v_sim[1][sim_idx][sel_target_tag];
            drone_traj2dof_data.vel_ned(1, i)  =  O.v_sim[0][sim_idx][sel_target_tag];
            drone_traj2dof_data.accl_ned(0, i) =  O.a_sim[1][sim_idx][sel_target_tag];
            drone_traj2dof_data.accl_ned(1, i) =  O.a_sim[0][sim_idx][sel_target_tag];
        }

        // ..:: Build GUI trajectory for visualization ::..
        // Do not display new trajectories if executing
        // sent trajectory. Needed because sometimes compute
        // overlaps with setting live reference mode
        if (this->model_->isLiveReference() || !this->getRunFlag()) continue;

        // Loop through each pooled target
        QVector<QPointF> empty_trajectory = QVector<QPointF>();
        for (int i = 0; i < skyenet::MAX_TARGETS; i++) {
            if (i < this->num_pooled_targets_) {
                // Solution points
                QVector<QPointF> trajectory_sol = QVector<QPointF>();

                // Add points to GUI trajectory
                for (quint32 j = 0; j < size_sol; j++) {
                    QVector3D gui_coords = xyzToGuiXyz(O.r[0][j][i],
                                                       O.r[1][j][i],
                                                       O.r[2][j][i]);
                    trajectory_sol.append(QPointF(gui_coords.x(),
                                                  gui_coords.y()));
                }
                // Set points on graphical display
                if (i == sel_target_tag) {
                    this->getTrajSolGraphic()->model_->setPoints(trajectory_sol);
                    this->getPooledTrajSolGraphic(i)->model_->setPoints(empty_trajectory);
                } else {
                    this->getPooledTrajSolGraphic(i)->model_->setPoints(trajectory_sol);
                }

                // Simulation points
                QVector<QPointF> trajectory_sim = QVector<QPointF>();

                // Add points to GUI trajectory
                for (quint32 j = 0; j < size_sim; j++) {
                    QVector3D gui_coords = xyzToGuiXyz(O.r_sim[0][j][i],
                                                       O.r_sim[1][j][i],
                                                       O.r_sim[2][j][i]);
                    trajectory_sim.append(QPointF(gui_coords.x(),
                                                  gui_coords.y()));
                }

                // Set points on graphical display
                if (i == sel_target_tag) {
                    this->getTrajSimGraphic()->model_->setPoints(trajectory_sim);
                    this->getPooledTrajSimGraphic(i)->model_->setPoints(empty_trajectory);
                } else {
                    this->getPooledTrajSimGraphic(i)->model_->setPoints(trajectory_sim);
                }

                bool is_feasible;
                if (!O.ddto_converged) {
                    // infeasible traj, set feasibility  code and traj color to red
                    if (i == sel_target_tag) {
                        this->model_->setIsValidTraj(FEASIBILITY_CODE::INFEASIBLE);
                    }
                    is_feasible = false;
                } else {
                    // feasible traj, set feasibility code and traj color to nominal
                    if (i == sel_target_tag) {
                        this->model_->setIsValidTraj(FEASIBILITY_CODE::FEASIBLE);
                    }
                    is_feasible = true;
                }
                emit updateMessage(this->drone_->model_);

                if (i == sel_target_tag) {
                    this->setFeasibilityColor(is_feasible);
                } else {
                    this->setPooledFeasibilityColor(is_feasible, i);
                }
            } else {
                // No valid trajectory (unallocated, set empty trajectory)
                this->getPooledTrajSolGraphic(i)->model_->setPoints(empty_trajectory);
                this->getPooledTrajSimGraphic(i)->model_->setPoints(empty_trajectory);
            }
        }

        // Send 2 DoF trajectory packet
        this->model_->setCurrTraj2dof(this->drone_->model_,
                                        drone_traj2dof_data);
    }
}

void ComputeThread::setFeasibilityColor(bool is_feasible) {
    // get graphics items
    DroneGraphicsItem *drone = this->getDroneGraphic();
    PathGraphicsItem *traj_sol = this->getTrajSolGraphic();
    PathGraphicsItem *traj_sim = this->getTrajSimGraphic();

    // set feasiblility color
    if (is_feasible) {
        traj_sol->setColor(YELLOW);
        traj_sim->setColor(YELLOW);
        drone->setIsFeasible(true);
    } else {
        traj_sol->setColor(RED);
        traj_sim->setColor(RED);
        drone->setIsFeasible(false);
    }
    emit updateGraphics(traj_sol, traj_sim, drone);
}

void ComputeThread::setPooledFeasibilityColor(bool is_feasible, quint32 tag) {
    // get graphics items
    DroneGraphicsItem *drone = this->getDroneGraphic();
    PathGraphicsItem *traj_sol = this->getPooledTrajSolGraphic(tag);
    PathGraphicsItem *traj_sim = this->getPooledTrajSimGraphic(tag);

    // set feasiblility color
    if (is_feasible) {
        traj_sol->setColor(TRANS_YELLOW);
        traj_sim->setColor(TRANS_YELLOW);
    } else {
        traj_sol->setColor(TRANS_RED);
        traj_sim->setColor(TRANS_RED);
    }
    emit updateGraphics(traj_sol, traj_sim, drone);
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
