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
//      void rx_keyboard_data(const packet::qcontrol_cmd& qcontrol_cmd);
//      void rx_keyboard_data(const packet::rcontrol_cmd& rcontrol_cmd);
//      void rx_heartbeat(const packet::heartbeat& heartbeat);
    signals:
      void tx_data(const packet::telemetry& telemetry);
  };

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
