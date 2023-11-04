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
                           QVector<PathGraphicsItem*> *traj_graphic);
    ~ComputeThread();

    PathGraphicsItem *getTrajGraphic(int);
    void setTarget(PointModelItem *target);
    void reInit();
    PointModelItem *getTarget();
    void stopCompute();
    DroneGraphicsItem *getDroneGraphic();

 protected:
    void run() override;

 // slots for signals from thread are run in parent thread
 signals:
    void updateGraphics(PathGraphicsItem *traj_graphic,
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
    QVector<PathGraphicsItem*> *traj_graphic_;
    int active_traj_;

    // compute traj flag
    bool run_loop_;

    // lock for accessing resources shared by compute_thread and controller
    // (target and run flag)
    QMutex mutex_;

    // flag to reset inputs
    bool target_changed_;

    INPUT_CODE validateInputs(QVector<QRegion> const &ellipse_regions,
                              QVector3D const &initial_pos,
                              QVector3D const &final_pos);
    void setFeasibilityColor(bool is_feasible);

    bool getRunFlag();
};

}  // namespace optgui

#endif  // COMPUTE_THREAD_H_
