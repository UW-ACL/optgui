// TITLE:   Optimization_Interface/src/network/item_server.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/network/item_server.h"

namespace interface {

ItemServer::ItemServer(DataModel *model, QObject *parent)
    : QTcpServer(parent) {
    this->model_ = model;

    this->connect(this, SIGNAL(newConnection()), this, SLOT(interact()));
}

bool ItemServer::startServer() {
    if (!this->listen(QHostAddress::Any, this->model_->port_)) {
        // qDebug() << this->errorString();
        this->close();
        return false;
    } else {
        // qDebug() << this->serverAddress() << this->serverPort();
        return true;
    }
}



}  // namespace interface
