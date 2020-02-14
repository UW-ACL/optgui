// TITLE:   Optimization_Interface/include/network/point_socket.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// UDP Socket for interacting with point
// constraint model over network

#ifndef POINT_SOCKET_H_
#define POINT_SOCKET_H_

#include <QUdpSocket>

#include "autogen/lib.h"

#include "include/models/point_model_item.h"

namespace optgui {

class PointSocket : public QUdpSocket {
    Q_OBJECT

 public:
    explicit PointSocket(PointModelItem *model, QObject *parent = nullptr);
    ~PointSocket();
    PointModelItem *point_model_;

 signals:
    void refresh_graphics();

 private slots:
    void readPendingDatagrams();
};

}  // namespace optgui

#endif  // POINT_SOCKET_H_

