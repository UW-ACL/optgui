// TITLE:   Optimization_Interface/src/network/ellipse_socket.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/network/ellipse_socket.h"

#include "include/globals.h"

namespace optgui {

EllipseSocket::EllipseSocket(EllipseGraphicsItem *item, QObject *parent)
    : QUdpSocket(parent) {
    this->ellipse_item_ = item;
    this->bind(QHostAddress::AnyIPv4, this->ellipse_item_->model_->port_);

    // automatically read incoming data with slots
    connect(this, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

EllipseSocket::~EllipseSocket() {
    // close UDP socket
    this->close();
}

void EllipseSocket::readPendingDatagrams() {
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
                QPointF gui_coords =
                        nedToGuiXyz(telemetry_data.pos_ned(0),
                                    telemetry_data.pos_ned(1));
                // set model pos
                this->ellipse_item_->model_->setPos(gui_coords);
                // set graphics pos so view knows whether to paint it
                this->ellipse_item_->setPos(gui_coords);
                // re-render graphics
                emit refresh_graphics();
            }
        }
    }
}

}  // namespace optgui
