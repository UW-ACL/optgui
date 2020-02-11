// TITLE:   Optimization_Interface/src/network/ellipse_socket.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/network/ellipse_socket.h"

#include "include/globals.h"

namespace optgui {

EllipseSocket::EllipseSocket(EllipseModelItem *model, QObject *parent)
    : QUdpSocket(parent) {
    ellipse_model_ = model;
    this->bind(QHostAddress::AnyIPv4, ellipse_model_->port_);

    connect(this, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

EllipseSocket::~EllipseSocket() {
    this->close();
}

void EllipseSocket::readPendingDatagrams() {
    while (this->hasPendingDatagrams()) {
        char buffer[4000] = {0};
        QHostAddress address;
        quint16 port;
        int64 bytes_read = this->readDatagram(buffer, 4000, &address, &port);

        if (bytes_read > 0) {
            autogen::deserializable::telemetry
                  <autogen::topic::telemetry::UNDEFINED> telemetry_data;
            const uint8 *ptr_telemetry_data =
                    telemetry_data.deserialize(
                        reinterpret_cast<const uint8 *>(buffer));
            if (ptr_telemetry_data != NULL) {
                QPointF pos(telemetry_data.pos_ned(1) * GRID_SIZE,
                            telemetry_data.pos_ned(0) * GRID_SIZE * -1);
                this->ellipse_model_->setPos(pos);
                emit refresh_graphics();
            }
        }
    }
}

}  // namespace optgui
