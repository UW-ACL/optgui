// TITLE:   Optimization_Interface/src/network/drone_socket.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/network/drone_socket.h"

#include "include/globals.h"

namespace interface {

DroneSocket::DroneSocket(DroneModelItem *model, QObject *parent)
    : QUdpSocket(parent) {
    drone_model_ = model;
    this->bind(QHostAddress::AnyIPv4, drone_model_->port_);

    connect(this, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

DroneSocket::~DroneSocket() {
    this->close();
}

void DroneSocket::readPendingDatagrams() {
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
              this->drone_model_->pos_->setX(telemetry_data.pos_ned(1) *
                                             GRID_SIZE);
              this->drone_model_->pos_->setY(telemetry_data.pos_ned(0) *
                                             GRID_SIZE * -1);
              emit refresh_graphics();
            }
        }
    }
}

void DroneSocket::rx_trajectory(const autogen::packet::traj3dof* data) {
    autogen::serializable::traj3dof
            <autogen::topic::traj3dof::UNDEFINED> ser_data;
    ser_data = *data;
    char buffer[4096] = {0};
    ser_data.serialize(reinterpret_cast<uint8 *>(buffer));

    // TODO(dtsull16): use config IP address
    this->writeDatagram(buffer, ser_data.size(),
                          QHostAddress("192.168.1.101"), 6000);
}

}  // namespace interface
