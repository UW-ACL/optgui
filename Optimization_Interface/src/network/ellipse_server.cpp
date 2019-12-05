// TITLE:   Optimization_Interface/src/network/ellipse_server.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/network/ellipse_server.h"

#include <QTcpSocket>
#include <QDataStream>

#include "include/globals.h"

namespace interface {

EllipseServer::EllipseServer(EllipseModelItem *model)
    : ItemServer(model) {
    ellipse_model_ = model;
}

void EllipseServer::interact() {
        QByteArray block;
        QDataStream in(&block, QIODevice::ReadOnly);
        in.setVersion(VERSION_5_8);

        QTcpSocket *client_connection = this->nextPendingConnection();
        connect(client_connection, SIGNAL(disconnected()),
                client_connection, SLOT(deleteLater()));

        // TODO(Miki): interact with ellipse model over network

        client_connection->disconnectFromHost();
}

}  // namespace interface
