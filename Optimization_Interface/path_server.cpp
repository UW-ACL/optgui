// TITLE:   Optimization_Interface/path_server.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "path_server.h"

#include <QTcpSocket>
#include <QDataStream>

namespace interface {

PathServer::PathServer(PathModelItem *model)
    : ItemServer(model) {
    path_model_ = model;
}

void PathServer::interact() {
        QByteArray block;
        QDataStream in(&block, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_11);

        QTcpSocket *client_connection = this->nextPendingConnection();
        connect(client_connection, SIGNAL(disconnected()),
                client_connection, SLOT(deleteLater()));

        // TODO: interact with path model over network

        client_connection->disconnectFromHost();
}

}  // namespace interface
