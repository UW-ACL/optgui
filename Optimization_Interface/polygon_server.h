// TITLE:   Optimization_Interface/polygon_server.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// TCP Server for interacting with polygon
// constraint model over network

#ifndef POLYGON_SERVER_H_
#define POLYGON_SERVER_H_

#include <QTcpSocket>
#include <QDataStream>

#include "item_server.h"
#include "polygon_model_item.h"

namespace interface {

class PolygonServer : public ItemServer {
    Q_OBJECT

 public:
    explicit PolygonServer(PolygonModelItem *model);
 private slots:
    void interact() override;
 private:
    PolygonModelItem *polygon_model_;
};

}  // namespace interface

#endif  // POLYGON_SERVER_H_
