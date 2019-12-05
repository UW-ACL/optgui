// TITLE:   Optimization_Interface/include/network/ellipse_server.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// TCP Server for interacting with ellipse
// constraint model over network

#ifndef ELLIPSE_SERVER_H_
#define ELLIPSE_SERVER_H_

#include "include/network/item_server.h"
#include "include/models/ellipse_model_item.h"

namespace interface {

class EllipseServer : public ItemServer {
    Q_OBJECT

 public:
    explicit EllipseServer(EllipseModelItem *model);

 private slots:
    void interact() override;

 private:
    EllipseModelItem *ellipse_model_;
};

}  // namespace interface

#endif  // ELLIPSE_SERVER_H_
