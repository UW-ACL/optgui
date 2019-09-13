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
      if (ptr_telemetry_data not_eq ptr) {
          emit tx_pos(telemetry_data.pos_ned(0), telemetry_data.pos_ned(1), telemetry_data.pos_ned(2));
      }
    }
  }
}

//this transmits a trajectory to the drones IP and port
void comm::rx_trajectory(const packet::traj3dof* data) {
    qDebug() << "rx_trajectory";
    serializable::traj3dof<topic::traj3dof::UNDEFINED> ser_data;
    ser_data = *data;
    uint8 buffer[4096]={0,};
    ser_data.serialize(buffer);

    char* buff = (char*)buffer;

//    packet::traj6dof* ser_data = new packet::traj6dof;

//    qDebug() << "sanity";
//    uint8* buffer = (uint8*)malloc(10000);
//    ser_data->serialize_content(buffer);//->serialize(buffer);
//    free(buffer);
//    buffer = nullptr;
    mp_udp->writeDatagram(buff,ser_data.size(),QHostAddress("192.168.1.101"),6000); //this is the IP/port of drone
//    mp_udp->writeDatagram(buff,ser_data.size(),QHostAddress("192.168.1.102"),6000);
//    mp_udp->writeDatagram(buff,ser_data.size(),QHostAddress("192.168.1.103"),6000);
//    mp_udp->writeDatagram(buff,ser_data.size(),QHostAddress("192.168.1.104"),6000);
    mp_udp->writeDatagram(buff,ser_data.size(),QHostAddress("192.168.1.105"),6000);
    mp_udp->writeDatagram(buff,ser_data.size(),QHostAddress("192.168.1.106"),6000);
    mp_udp->writeDatagram(buff,ser_data.size(),QHostAddress("192.168.1.107"),6000);
    mp_udp->writeDatagram(buff,ser_data.size(),QHostAddress("192.168.1.108"),6000);
    mp_udp->writeDatagram(buff,ser_data.size(),QHostAddress("192.168.1.109"),6000);
    mp_udp->writeDatagram(buff,ser_data.size(),QHostAddress("192.168.1.110"),6000);

}


////////////////////////////////////////////////////////////////////////////////////////////////////
