// TITLE:   Optimization_Interface/include/network/drone_socket.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// UDP Socket for interacting with drone over network

#ifndef DRONE_SOCKET_H_
#define DRONE_SOCKET_H_

#include <QUdpSocket>

#include "autogen/lib.h"

#include "include/models/drone_model_item.h"

namespace interface {

class DroneSocket : public QUdpSocket {
    Q_OBJECT

 public:
    explicit DroneSocket(DroneModelItem *model, QObject *parent = nullptr);
    ~DroneSocket();
    DroneModelItem *drone_model_;

 private slots:
    void readPendingDatagrams();

 signals:
    void refresh_graphics();

 public slots:
    void rx_trajectory(const autogen::packet::traj3dof *data);

 private:
    bool isDestinationAddrValid();
};

}  // namespace interface

#endif  // DRONE_SOCKET_H_
