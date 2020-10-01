// TITLE:   Optimization_Interface/src/network/point_socket.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/network/point_socket.h"

#include "include/globals.h"

namespace optgui {

PointSocket::PointSocket(PointGraphicsItem *item, QObject *parent)
    : QUdpSocket(parent) {
    this->point_item_ = item;
    this->bind(QHostAddress::AnyIPv4, this->point_item_->model_->port_);

    // automatically read incoming data with slots
    connect(this, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

PointSocket::~PointSocket() {
    // close UDP socket
    this->close();
}

void PointSocket::readPendingDatagrams() {
    while (this->hasPendingDatagrams()) {
        // create empty buffer
        char buffer[4000] = {0};
        QHostAddress address;
        quint16 port;
        int64 bytes_read = this->readDatagram(buffer, 4000, &address, &port);

        if (bytes_read > 0) {
            // deserialzie data
            autogen::deserializable::telemetry
                  <autogen::topic::telemetry::UNDEFINED> telemetry_data;
            // pointer is NULL if does not deserialize correctly
            const uint8 *ptr_telemetry_data =
                    telemetry_data.deserialize(
                        reinterpret_cast<const uint8 *>(buffer));
            if (ptr_telemetry_data != NULL) {
                QVector3D gui_coords_3D =
                        nedToGuiXyz(telemetry_data.pos_ned(0),
                                    telemetry_data.pos_ned(1),
                                    telemetry_data.pos_ned(2));
                QPointF gui_coords_2D = QPointF(gui_coords_3D.x(),
                                                gui_coords_3D.y());
                // set model coords
                this->point_item_->model_->setPos(gui_coords_2D);
                // set graphics coords so view knows to render it
                this->point_item_->setPos(gui_coords_2D);
                emit refresh_graphics();
            }
        }
    }
}

}  // namespace optgui
