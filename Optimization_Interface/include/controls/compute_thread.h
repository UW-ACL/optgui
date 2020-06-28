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
#include "include/globals.h"

namespace optgui {

class ComputeThread : public QThread {
    Q_OBJECT

 public:
    explicit ComputeThread(ConstraintModel *model);
    ~ComputeThread();

 protected:
    // main thread loop
    void run() override;

 // slots for signals from thread are run in parent thread
 signals:
    // render graphics
    void updateGraphics();
    // set color of traj
    void setPathColor(bool isRed);
    // update feasibility message box
    void updateMessage();

 public slots:
    // stop main loop from continuing to execute
    // so thread can close
    void stopCompute();

 private:
    // pointer to model
    ConstraintModel *model_;
    // solver
    skyenet::SkyeFly *fly_;
    // flag to continue running loop
    bool run_loop_;
    // validate inputs from model
    INPUT_CODE validateInputs(QVector<QRegion> const &ellipse_regions,
                              QPointF const &initial_pos,
                              QPointF const &final_pos);
};

}  // namespace optgui

#endif  // COMPUTE_THREAD_H_
