// TITLE:   Optimization_Interface/src/network/waypoint_socket.cpp
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

#include "include/network/waypoint_socket.h"

#include "include/globals.h"

namespace optgui {

WaypointSocket::WaypointSocket(WaypointGraphicsItem *item, QObject *parent)
    : QUdpSocket(parent) {
    this->waypoint_item_ = item;
    this->bind(QHostAddress::AnyIPv4, this->waypoint_item_->model_->port_);

    connect(this, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

WaypointSocket::~WaypointSocket() {
    this->close();
}

void WaypointSocket::readPendingDatagrams() {
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
                QPointF gui_coords =
                        nedToGuiXyz(telemetry_data.pos_ned(0),
                                    telemetry_data.pos_ned(1));
                // set model coords
                this->waypoint_item_->model_->setPos(gui_coords);
                // set graphics coords so view knows to render it
                this->waypoint_item_->setPos(gui_coords);
                emit refresh_graphics();
            }
        }
    }
}

}  // namespace optgui
