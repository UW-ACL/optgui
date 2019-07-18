// TITLE:   Optimization_Interface/polygon_server.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "polygon_server.h"

#include <QTcpSocket>
#include <QDataStream>

#include "globals.h"

namespace interface {

PolygonServer::PolygonServer(PolygonModelItem *model)
    : ItemServer(model) {
    polygon_model_ = model;
}

void PolygonServer::interact() {
        QByteArray block;
        QDataStream in(&block, QIODevice::ReadOnly);
        in.setVersion(VERSION_5_8);

        QTcpSocket *client_connection = this->nextPendingConnection();
        connect(client_connection, SIGNAL(disconnected()),
                client_connection, SLOT(deleteLater()));

        // TODO(Miki): interact with polygon model over network

        client_connection->disconnectFromHost();
}

}  // namespace interface
