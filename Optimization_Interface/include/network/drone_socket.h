// TITLE:   Optimization_Interface/include/network/drone_socket.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// UDP Socket for interacting with drone over network

#ifndef DRONE_SOCKET_H_
#define DRONE_SOCKET_H_

#include <QUdpSocket>

#include "autogen/lib.h"

#include "include/graphics/drone_graphics_item.h"

namespace optgui {

class DroneSocket : public QUdpSocket {
    Q_OBJECT

 public:
    explicit DroneSocket(DroneGraphicsItem *item, QObject *parent = nullptr);
    ~DroneSocket();

    // graphic item to manipulate over network
    DroneGraphicsItem *drone_item_;

 private slots:
    // automatically read incoming data with slots
    void readPendingDatagrams();

 signals:
    // signal to re-render vehicle
    void refresh_graphics();

 public slots:
    void rx_trajectory(DroneModelItem *drone,
                       const autogen::packet::traj2dof data);

 private:
    // check if destination address is valid
    bool isDestinationAddrValid();
};

}  // namespace optgui

#endif  // DRONE_SOCKET_H_
