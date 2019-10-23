// TITLE:   Optimization_Interface/include/network/drone_server.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// TCP Server for interacting with drone over network

#ifndef DRONE_SERVER_H_
#define DRONE_SERVER_H_

#include "item_server.h"
#include "../models/drone_model_item.h"

namespace interface {

class DroneServer : public ItemServer {
    Q_OBJECT

 public:
    explicit DroneServer(DroneModelItem *model);

 private slots:
    void interact() override;

 private:
    DroneModelItem *drone_model_;
};

}  // namespace interface

#endif  // DRONE_SERVER_H_
