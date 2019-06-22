#ifndef COMM_H
#define COMM_H

#include "autogen/lib.h"
#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QString>
#include <QThread>
#include <QUdpSocket>

using namespace autogen;

  class comm : public QObject {
    Q_OBJECT
    private:
      const std::string mc_ip_address;
      const uint16 mc_port;
      QUdpSocket* mp_udp;
    public:
      comm(const std::string& ip_address, uint16 port);
      ~comm();
    public slots:
      void readPendingDatagrams();
      void rx_trajectory(const packet::traj3dof* data);
    signals:
      void tx_pos(float n, float e, float d);
  };
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
