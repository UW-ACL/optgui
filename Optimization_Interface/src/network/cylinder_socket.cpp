// TITLE:   Optimization_Interface/src/network/cylinder_socket.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/network/cylinder_socket.h"

#include "include/globals.h"

namespace optgui {

CylinderSocket::CylinderSocket(CylinderGraphicsItem *item, QObject *parent)
    : QUdpSocket(parent) {
    this->cylinder_item_ = item;
    this->bind(QHostAddress::AnyIPv4, this->cylinder_item_->model_->port_);

    // automatically read incoming data with slots
    connect(this, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

CylinderSocket::~CylinderSocket() {
    // close UDP socket
    this->close();
}

void CylinderSocket::readPendingDatagrams() {
    while (this->hasPendingDatagrams()) {
        // create empty buffer
        char buffer[4000] = {0};
        QHostAddress address;
        quint16 port;
        int64 bytes_read = this->readDatagram(buffer, 4000, &address, &port);

        if (bytes_read > 0) {
            // deserialize telemetry
            autogen::deserializable::telemetry
                  <autogen::topic::telemetry::UNDEFINED> telemetry_data;
            // pointer is NULL if does not deserialize correctly
            const uint8 *ptr_telemetry_data =
                    telemetry_data.deserialize(
                        reinterpret_cast<const uint8 *>(buffer));
            if (ptr_telemetry_data != NULL) {
                // Position
                QVector3D gui_coords_3D =
                        nedToGuiXyz(telemetry_data.pos_ned(0),
                                    telemetry_data.pos_ned(1),
                                    telemetry_data.pos_ned(2));
                QPointF gui_coords_2D = QPointF(gui_coords_3D.x(),
                                                gui_coords_3D.y());

                // Rotation
                gnc::quat gui_rotations_3D = telemetry_data.q_i2b;
                qreal yaw = gui_rotations_3D.yaw_deg();

                // set model pos and rotation
                this->cylinder_item_->model_->setPos(gui_coords_2D);
                this->cylinder_item_->model_->setRot(yaw);
                // set graphics pos so view knows whether to paint it
                this->cylinder_item_->setPos(gui_coords_2D);
                this->cylinder_item_->setRotation(yaw);
                emit refresh_graphics();
            }
        }
    }
}

}  // namespace optgui
