// TITLE:   Optimization_Interface/include/controls/compute_thread.h
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

// Thread for concurrently running Skyfly compute

#ifndef COMPUTE_THREAD_H_
#define COMPUTE_THREAD_H_

#include <QThread>

#include "cprs.h"
#include "algorithm.h"

#include "include/models/constraint_model.h"
#include "include/graphics/path_graphics_item.h"
#include "include/graphics/drone_graphics_item.h"
#include "include/globals.h"

namespace optgui {

class ComputeThread : public QThread {
    Q_OBJECT

 public:
    explicit ComputeThread(ConstraintModel *model,
                           DroneGraphicsItem *drone,
                           PathGraphicsItem *traj_graphic_sol,
                           PathGraphicsItem *traj_graphic_sim,
                           PathGraphicsItem *traj_graphic_sol_pool[skyenet::MAX_TARGETS],
                           PathGraphicsItem *traj_graphic_sim_pool[skyenet::MAX_TARGETS]);
    ~ComputeThread();

    PathGraphicsItem *getTrajSolGraphic();
    PathGraphicsItem *getTrajSimGraphic();
    PathGraphicsItem *getPooledTrajSolGraphic(quint32 tag);
    PathGraphicsItem *getPooledTrajSimGraphic(quint32 tag);
    void setTarget(PointModelItem *target);
   //  void addPooledTarget(PointModelItem *target);
    void setPooledTargets(PointModelItem* targets[skyenet::MAX_TARGETS], quint32 num_targets);
    void reInit();
    PointModelItem *getTarget();
    PointModelItem *getPooledTarget(quint32 tag);
   //  void checkRemovedTarget();
    void stopCompute();
    void initializeJulia();
    DroneGraphicsItem *getDroneGraphic();

 protected:
    void run() override;

 // slots for signals from thread are run in parent thread
 signals:
    void updateGraphics(PathGraphicsItem *traj_graphic_sol,
                        PathGraphicsItem *traj_graphic_sim,
                        DroneGraphicsItem *drone_graphic);
    void updateMessage(DroneModelItem *drone);
    void finalTime(DroneModelItem *drone, double final_time);

 private:
    // GUI data
    ConstraintModel *model_;
    // problem data
    skyenet::SkyeFly fly_;

    // vehicle and target
    DroneGraphicsItem *drone_;
    PointModelItem *target_;
    PathGraphicsItem *traj_graphic_sol_;
    PathGraphicsItem *traj_graphic_sim_;

    // target pool (options) for multitarget traj opt (e.g. DDTO)
    PointModelItem *target_pool_[skyenet::MAX_TARGETS];
    PathGraphicsItem *traj_graphic_sol_pool_[skyenet::MAX_TARGETS];
    PathGraphicsItem *traj_graphic_sim_pool_[skyenet::MAX_TARGETS];

    // compute traj flag
    bool run_loop_;

    // lock for accessing resources shared by compute_thread and controller
    // (target and run flag)
    QMutex mutex_;

    // flag to reset inputs
    bool target_changed_;

    bool julia_initialized_;

    // Keeps count of the number of targets in consideration
    quint32 num_pooled_targets_;

    INPUT_CODE validateInputs(QVector<QRegion> const &ellipse_regions,
                              QVector3D const &initial_pos,
                              QVector3D const &final_pos);
    void setFeasibilityColor(bool is_feasible);
    void setPooledFeasibilityColor(bool is_feasible, quint32 tag);

    bool getRunFlag();
};

}  // namespace optgui

#endif  // COMPUTE_THREAD_H_
