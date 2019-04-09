// TITLE:   Optimization_Interface/plane_server.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// TCP Server for interacting with plane
// constraint model over network

#ifndef PLANE_SERVER_H_
#define PLANE_SERVER_H_

#include "item_server.h"
#include "plane_model_item.h"

namespace interface {

class PlaneServer : public ItemServer {
    Q_OBJECT

 public:
    explicit PlaneServer(PlaneModelItem *model);
 private slots:
    void interact() override;
 private:
    PlaneModelItem *plane_model_;
};

}  // namespace interface

#endif  // plane_SERVER_H_
