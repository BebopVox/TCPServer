#pragma once

#include "C_NetPacket.h"

class C_TCPServerNetwork;

class C_TCPServer
{
public:
  C_TCPServer();
  ~C_TCPServer();

  void update();
  void receiveFromClients();
  void sendActionPackets();

private:
  // IDs for the clients connecting for table in ServerNetwork 
  unsigned int m_uiLastConnectedID;

  // The ServerNetwork object 
  C_TCPServerNetwork* m_pNetwork;

  // data buffer
  char m_szData[C_NetPacket::MAX_PACKET_SIZE];
};
