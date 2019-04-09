// TITLE:   Optimization_Interface/item_server.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// TCP Server for starting and stopping server
// with generic item over network

#ifndef ITEM_SERVER_H_
#define ITEM_SERVER_H_

#include <QTcpServer>

#include "data_model.h"

namespace interface {

class ItemServer : public QTcpServer {
    Q_OBJECT

 public:
    ItemServer(DataModel *model, QObject *parent = nullptr);
    virtual bool startServer();
 private slots:
    virtual void interact() = 0;
 private:
    DataModel *model_;
};

}  // namespace interface

#endif  // ITEM_SERVER_H_
