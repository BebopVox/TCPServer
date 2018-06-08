#pragma once

#include <WinSock2.h>
#include <map>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class C_TCPServerNetwork
{
public:
  C_TCPServerNetwork(void);
  ~C_TCPServerNetwork(void);

  // send data to all clients
  void sendToAll(char* pszPackets, int iTotalSize);
  // receive incoming data
  int receiveData(unsigned int uiClientID, char* pszRecvBuf);
  // accept new connections
  bool acceptNewClient(unsigned int &uiID);

//private:
  // Socket to listen for new connections
  SOCKET m_SocketListen;

  // Socket to give to the clients
  SOCKET m_SocketClient;

  // for error checking return values
  int m_iResult;

  // table to keep track of each client's socket
  map<unsigned int, SOCKET> m_mapSeccions; 
};
