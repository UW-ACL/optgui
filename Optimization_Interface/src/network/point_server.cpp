// TITLE:   Optimization_Interface/src/network/point_server.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/network/point_server.h"

#include <QTcpSocket>
#include <QDataStream>

#include "include/globals.h"

namespace interface {

PointServer::PointServer(PointModelItem *model)
    : ItemServer(model) {
    point_model_ = model;
}

void PointServer::interact() {
        QByteArray block;
        QDataStream in(&block, QIODevice::ReadOnly);
        in.setVersion(VERSION_5_8);

        QTcpSocket *client_connection = this->nextPendingConnection();
        connect(client_connection, SIGNAL(disconnected()),
                client_connection, SLOT(deleteLater()));

        // TODO(Miki): interact with point model over network

        client_connection->disconnectFromHost();
}

}  // namespace interface
