// TITLE:   Optimization_Interface/src/network/point_socket.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/network/point_socket.h"

#include "include/globals.h"

namespace interface {

PointSocket::PointSocket(PointModelItem *model, QObject *parent)
    : QUdpSocket(parent) {
    point_model_ = model;
    this->bind(QHostAddress::AnyIPv4, point_model_->port_);

    connect(this, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

PointSocket::~PointSocket() {
    this->close();
}

void PointSocket::readPendingDatagrams() {
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
              this->point_model_->pos_->setX(telemetry_data.pos_ned(1) *
                                             GRID_SIZE);
              this->point_model_->pos_->setY(telemetry_data.pos_ned(0) *
                                             GRID_SIZE * -1);
              emit refresh_graphics();
            }
        }
    }
}

}  // namespace interface