// TITLE:   Optimization_Interface/include/network/path_server.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// TCP Server for interacting with drone path or
// waypoints over network

#ifndef PATH_SERVER_H_
#define PATH_SERVER_H_

#include "include/network/item_server.h"
#include "include/models/path_model_item.h"

namespace interface {

class PathServer : public ItemServer {
    Q_OBJECT

 public:
    explicit PathServer(PathModelItem *model);

 private slots:
    void interact() override;

 private:
    PathModelItem *path_model_;
};

}  // namespace interface

#endif  // PATH_SERVER_H_
