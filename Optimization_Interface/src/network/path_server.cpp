// TITLE:   Optimization_Interface/src/network/path_server.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/network/path_server.h"

#include <QTcpSocket>
#include <QDataStream>

#include "include/globals.h"

namespace interface {

PathServer::PathServer(PathModelItem *model)
    : ItemServer(model) {
    path_model_ = model;
}

void PathServer::interact() {
        QByteArray block;
        QDataStream in(&block, QIODevice::ReadOnly);
        in.setVersion(VERSION_5_8);

        QTcpSocket *client_connection = this->nextPendingConnection();
        connect(client_connection, SIGNAL(disconnected()),
                client_connection, SLOT(deleteLater()));

        // TODO(dtsull): interact with path model over network

        client_connection->disconnectFromHost();
}

}  // namespace interface
