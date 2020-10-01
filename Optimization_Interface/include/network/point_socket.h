// TITLE:   Optimization_Interface/include/network/point_socket.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// UDP Socket for interacting with target point over network

#ifndef POINT_SOCKET_H_
#define POINT_SOCKET_H_

#include <QUdpSocket>

#include "autogen/lib.h"

#include "include/graphics/point_graphics_item.h"

namespace optgui {

class PointSocket : public QUdpSocket {
    Q_OBJECT

 public:
    explicit PointSocket(PointGraphicsItem *item,
                         QObject *parent = nullptr);
    ~PointSocket();

    // target point to manipulate over network
    PointGraphicsItem *point_item_;

 signals:
    // signal to re-render target point graphic
    void refresh_graphics();

 private slots:
    // automatically read incoming data with slots
    void readPendingDatagrams();
};

}  // namespace optgui

#endif  // POINT_SOCKET_H_

