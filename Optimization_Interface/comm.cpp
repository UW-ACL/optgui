#include "comm.h"

comm::comm(const std::string& ip_address, uint16 port) : mc_ip_address(ip_address),mc_port(port) {
  // Instantiate UDP socket.
  mp_udp = new QUdpSocket(this);
  mp_udp->bind(QHostAddress::AnyIPv4,port);

  // Connect signals/slots.
  connect(mp_udp,SIGNAL(readyRead()),this,SLOT(readPendingDatagrams()));

  std::cout << "Init comm" << std::endl;
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

    if ((bytes_read > 0)) {
      uint8 buffer[4000]={0,};
      memcpy(buffer,buff,4000);
      const uint8* ptr = NULL;

      // Deserialize mocap data.
      ptr = buffer;
      deserializable::telemetry<topic::telemetry::UNDEFINED> telemetry_data;
      const uint8* ptr_telemetry_data = telemetry_data.deserialize(ptr);
//      std::cout << "Received bytes" << std::endl;
      if (ptr_telemetry_data not_eq ptr) {
//          std::cout << "Read mocap " << this->mc_port << ":" \
                    << mocap_data.pos_ned(0) << "," \
                    << mocap_data.pos_ned(1) << "," \
                    << mocap_data.pos_ned(2) << std::endl;
          emit tx_pos(telemetry_data.pos_ned(0), telemetry_data.pos_ned(1), telemetry_data.pos_ned(2));
      }

//      // Deserialize telemetry.
//      ptr = buffer;
//      deserializable::telemetry<topic::telemetry::UNDEFINED> telemetry;
//      const uint8* ptr_telemetry = telemetry.deserialize(ptr);
//      if (ptr_telemetry not_eq ptr) {
//        emit tx_data(telemetry);
//      }
    }
  }
}

//void comm::rx_keyboard_data(const packet::qcontrol_cmd& qcontrol_cmd) {
//  serializable::qcontrol_cmd<topic::qcontrol_cmd::UNDEFINED> ser_qcontrol_cmd;
//  ser_qcontrol_cmd = qcontrol_cmd;

//  uint8 buffer[1028]={0,};
//  char* buff = (char*)buffer;

//  // Serialize qcontrol_cmd (fm_trans_cmd + quad control mode + inputs).
//  ser_qcontrol_cmd.serialize(buffer);
//  mp_udp->writeDatagram(buff,ser_qcontrol_cmd.size(),QHostAddress(mc_ip_address.c_str()),mc_port);
//}
//void comm::rx_keyboard_data(const packet::rcontrol_cmd& rcontrol_cmd) {
//  serializable::rcontrol_cmd<topic::rcontrol_cmd::UNDEFINED> ser_rcontrol_cmd;
//  ser_rcontrol_cmd = rcontrol_cmd;

//  uint8 buffer[1028]={0,};
//  char* buff = (char*)buffer;

//  // Serialize rcontrol_cmd (fm_trans_cmd + rover control mode + inputs).
//  ser_rcontrol_cmd.serialize(buffer);
//  mp_udp->writeDatagram(buff,ser_rcontrol_cmd.size(),QHostAddress(mc_ip_address.c_str()),mc_port);
//}
//void comm::rx_heartbeat(const packet::heartbeat& heartbeat) {
//  serializable::heartbeat<topic::heartbeat::UNDEFINED> ser_heartbeat;
//  ser_heartbeat = heartbeat;

//  uint8 buffer[1028]={0,};
//    char* buff = (char*)buffer;
//    ser_heartbeat.serialize(buffer);
//    mp_udp->writeDatagram(buff,ser_heartbeat.size(),QHostAddress(mc_ip_address.c_str()),mc_port);
//}

////////////////////////////////////////////////////////////////////////////////////////////////////
