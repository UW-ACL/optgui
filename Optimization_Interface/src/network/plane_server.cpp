// TITLE:   Optimization_Interface/src/network/plane_server.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "../../include/network/plane_server.h"

#include <QTcpSocket>
#include <QDataStream>

#include "../../include/globals.h"

namespace interface {

PlaneServer::PlaneServer(PlaneModelItem *model)
    : ItemServer(model) {
    plane_model_ = model;
}

void PlaneServer::interact() {
        QByteArray block;
        QDataStream in(&block, QIODevice::ReadOnly);
        in.setVersion(VERSION_5_8);

        QTcpSocket *client_connection = this->nextPendingConnection();
        connect(client_connection, SIGNAL(disconnected()),
                client_connection, SLOT(deleteLater()));

        // TODO(Miki): interact with plane model over network

        client_connection->disconnectFromHost();
}

}  // namespace interface
