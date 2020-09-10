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
    DroneGraphicsItem *drone_item_;

 private slots:
    void readPendingDatagrams();

 signals:
    void refresh_graphics();

 public slots:
    void rx_trajectory(DroneModelItem *drone,
                       const autogen::packet::traj3dof data);

 private:
    bool isDestinationAddrValid();
};

}  // namespace optgui

#endif  // DRONE_SOCKET_H_
