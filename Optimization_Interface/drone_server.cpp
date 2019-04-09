// TITLE:   Optimization_Interface/drone_server.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "drone_server.h"

#include <QTcpSocket>
#include <QDataStream>

namespace interface {

DroneServer::DroneServer(DroneModelItem *model)
    : ItemServer(model) {
    drone_model_ = model;
}

void DroneServer::interact() {
        QByteArray block;
        QDataStream in(&block, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_11);

        QTcpSocket *client_connection = this->nextPendingConnection();
        connect(client_connection, SIGNAL(disconnected()),
                client_connection, SLOT(deleteLater()));

        // TODO(Miki): interact with drone model over network

        client_connection->disconnectFromHost();
}

}  // namespace interface
