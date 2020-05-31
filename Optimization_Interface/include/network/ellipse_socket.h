// TITLE:   Optimization_Interface/include/network/ellipse_socket.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// UDP Socket for interacting with ellipse
// constraint model over network

#ifndef ELLIPSE_SOCKET_H_
#define ELLIPSE_SOCKET_H_

#include <QUdpSocket>

#include "autogen/lib.h"

#include "include/graphics/ellipse_graphics_item.h"

namespace optgui {

class EllipseSocket : public QUdpSocket {
    Q_OBJECT

 public:
    explicit EllipseSocket(EllipseGraphicsItem *item,
                           QObject *parent = nullptr);
    ~EllipseSocket();
    EllipseGraphicsItem *ellipse_item_;

 signals:
    void refresh_graphics();

 private slots:
    void readPendingDatagrams();
};

}  // namespace optgui

#endif  // ELLIPSE_SOCKET_H_
