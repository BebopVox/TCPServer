#include <iostream>

#include "C_TCPServerNetwork.h"

#include "C_TCPServer.h"

C_TCPServer::C_TCPServer()
{
  // id's to assign clients for our table
  m_uiLastConnectedID = 0;

  // set up the server network to listen 
  m_pNetwork = new C_TCPServerNetwork(); 
}

C_TCPServer::~C_TCPServer()
{

}

void C_TCPServer::update()
{
  // get new clients
  if(m_pNetwork->acceptNewClient(m_uiLastConnectedID))
  {
    cout << "client " << m_uiLastConnectedID << " has been connected to the server" << endl;
    m_uiLastConnectedID++;
  }

  receiveFromClients();
}

void C_TCPServer::receiveFromClients()
{
  C_NetPacket packet;

  // go through all clients

  for(auto iter = m_pNetwork->m_mapSeccions.begin(); iter != m_pNetwork->m_mapSeccions.end(); iter++)
  {
    int iDataLength = m_pNetwork->receiveData(iter->first, m_szData);

    if (iDataLength < 1) 
    {
      //no data recieved
      continue;
    }

    for (unsigned int i = 0; i < (unsigned int)iDataLength; i += sizeof(C_NetPacket))
    {
      packet.Deserialize(&(m_szData[i]));

      switch (packet.m_uiType)
      {
        case C_NetPacket::PacketTypes::Init:
        {
          cout << "server: received init packet from client " << endl;
          sendActionPackets();
          break;
        }
        case C_NetPacket::PacketTypes::ActionEvent:
        {
          cout << "server: received action event packet from client" << endl;
          sendActionPackets();
          break;
        }
        default:
        {
          cout << "server: error in packet types" << endl;
          break;
        }
      }
    }
  }
}

void C_TCPServer::sendActionPackets()
{
  // send action packet
  const unsigned int packet_size = sizeof(C_NetPacket);
  char packet_data[packet_size];

  C_NetPacket packet;
  packet.m_uiType = C_NetPacket::PacketTypes::ActionEvent;
  strcpy_s(packet.m_szText, "Hello, TCP world!");

  packet.Serialize(packet_data);

  m_pNetwork->sendToAll(packet_data, packet_size);
}
