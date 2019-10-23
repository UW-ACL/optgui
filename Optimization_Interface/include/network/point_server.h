// TITLE:   Optimization_Interface/include/network/point_server.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// TCP Server for interacting with point
// constraint model over network

#ifndef POINT_SERVER_H_
#define POINT_SERVER_H_

#include "item_server.h"
#include "../models/point_model_item.h"

namespace interface {

class PointServer : public ItemServer {
    Q_OBJECT

 public:
    explicit PointServer(PointModelItem *model);

 private slots:
    void interact() override;

 private:
    PointModelItem *point_model_;
};

}  // namespace interface

#endif  // POINT_SERVER_H_
