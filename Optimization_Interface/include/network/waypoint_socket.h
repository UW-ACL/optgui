// TITLE:   Optimization_Interface/include/network/waypoint_socket.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// UDP Socket for interacting with point
// constraint model over network

#ifndef WAYPOINT_SOCKET_H_
#define WAYPOINT_SOCKET_H_

#include <QUdpSocket>

#include "autogen/lib.h"

#include "include/graphics/waypoint_graphics_item.h"

namespace optgui {

class WaypointSocket : public QUdpSocket {
    Q_OBJECT

 public:
    explicit WaypointSocket(WaypointGraphicsItem *item,
                            QObject *parent = nullptr);
    ~WaypointSocket();
    WaypointGraphicsItem *waypoint_item_;

 signals:
    void refresh_graphics();

 private slots:
    void readPendingDatagrams();
};

}  // namespace optgui

#endif  // WAYPOINT_SOCKET_H_

