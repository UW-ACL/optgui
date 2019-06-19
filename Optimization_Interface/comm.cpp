#include "comm.h"

comm::comm(const std::string& ip_address, uint16 port) : mc_ip_address(ip_address),mc_port(6000) {
  // Instantiate UDP socket.
  mp_udp = new QUdpSocket(this);
  mp_udp->bind(QHostAddress::AnyIPv4,port);

  // Connect signals/slots.
  connect(mp_udp,SIGNAL(readyRead()),this,SLOT(readPendingDatagrams()));
}
comm::~comm() {
  // Disconnect signals/slots.
  disconnect(mp_udp,SIGNAL(readyRead()),this,SLOT(readPendingDatagrams()));

  // Delete UDP socket.
  mp_udp->close();
  delete mp_udp;
}
void comm::readPendingDatagrams() {
  while (mp_udp->hasPendingDatagrams()) {
    char buff[4000]={0,};
    QHostAddress address;
    quint16 port;
    int64 bytes_read = mp_udp->readDatagram(buff,4000,&address,&port);

    if ((bytes_read > 0) and (port != 1508)) {
      uint8 buffer[4000]={0,};
      memcpy(buffer,buff,4000);
      const uint8* ptr = NULL;

      // Deserialize telemetry.
      ptr = buffer;
      deserializable::telemetry<topic::telemetry::UNDEFINED> telemetry;
      const uint8* ptr_telemetry = telemetry.deserialize(ptr);
      if (ptr_telemetry not_eq ptr) {
        emit tx_data(telemetry);
      }
    }
  }
}
void comm::rx_keyboard_data(const packet::qcontrol_cmd& qcontrol_cmd) {
  serializable::qcontrol_cmd<topic::qcontrol_cmd::UNDEFINED> ser_qcontrol_cmd;
  ser_qcontrol_cmd = qcontrol_cmd;

  uint8 buffer[1028]={0,};
  char* buff = (char*)buffer;

  // Serialize qcontrol_cmd (fm_trans_cmd + quad control mode + inputs).
  ser_qcontrol_cmd.serialize(buffer);
  mp_udp->writeDatagram(buff,ser_qcontrol_cmd.size(),QHostAddress(mc_ip_address.c_str()),mc_port);
}
void comm::rx_keyboard_data(const packet::rcontrol_cmd& rcontrol_cmd) {
  serializable::rcontrol_cmd<topic::rcontrol_cmd::UNDEFINED> ser_rcontrol_cmd;
  ser_rcontrol_cmd = rcontrol_cmd;

  uint8 buffer[1028]={0,};
  char* buff = (char*)buffer;

  // Serialize rcontrol_cmd (fm_trans_cmd + rover control mode + inputs).
  ser_rcontrol_cmd.serialize(buffer);
  mp_udp->writeDatagram(buff,ser_rcontrol_cmd.size(),QHostAddress(mc_ip_address.c_str()),mc_port);
}
void comm::rx_heartbeat(const packet::heartbeat& heartbeat) {
  serializable::heartbeat<topic::heartbeat::UNDEFINED> ser_heartbeat;
  ser_heartbeat = heartbeat;

  uint8 buffer[1028]={0,};
    char* buff = (char*)buffer;
    ser_heartbeat.serialize(buffer);
    mp_udp->writeDatagram(buff,ser_heartbeat.size(),QHostAddress(mc_ip_address.c_str()),mc_port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
