// TITLE:   Optimization_Interface/include/network/cylinder_socket.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// UDP Socket for interacting with cylinder
// STC over network

#ifndef CYLINDER_SOCKET_H_
#define CYLINDER_SOCKET_H_

#include <QUdpSocket>

#include "autogen/lib.h"

#include "include/graphics/cylinder_graphics_item.h"

namespace optgui {

class CylinderSocket : public QUdpSocket {
    Q_OBJECT

 public:
    explicit CylinderSocket(CylinderGraphicsItem *item,
                           QObject *parent = nullptr);
    ~CylinderSocket();

    // Cylinder STC to manipulate over network
    CylinderGraphicsItem *cylinder_item_;

 signals:
    // signal to re-render Cylinder
    void refresh_graphics();

 private slots:
    // automatically read incoming data with slots
    void readPendingDatagrams();
};

}  // namespace optgui

#endif  // CYLINDER_SOCKET_H_
