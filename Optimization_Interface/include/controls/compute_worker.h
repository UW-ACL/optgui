// TITLE:   Optimization_Interface/include/controls/compute_worker.h
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

// Thread for concurrently running Skyfly compute

#ifndef COMPUTE_WORKER_H_
#define COMPUTE_WORKER_H_

#include <cprs.h>
#include <algorithm.h>

#include "include/models/constraint_model.h"
#include "include/globals.h"

namespace interface {

class ComputeWorker : public QObject {
    Q_OBJECT

 public:
    explicit ComputeWorker(ConstraintModel *model);
    ~ComputeWorker();

 signals:
    void startCompute();
    void updateGraphics();
    // set color
    // set message
    // set valid

 private slots:
    void compute();

 private:
    ConstraintModel *model_;
    SkyeFly *fly_;

    void initializeFly();
};

}  // namespace interface

#endif  // COMPUTE_WORKER_H_
