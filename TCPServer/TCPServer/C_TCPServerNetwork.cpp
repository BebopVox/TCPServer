#include <iostream>
#include <ws2tcpip.h>

#include "C_TCPService.h"
#include "C_NetPacket.h"

#include "C_TCPServerNetwork.h"

C_TCPServerNetwork::C_TCPServerNetwork()
{
  // create WSADATA object
  WSADATA wsaData;

  // our sockets for the server
  m_SocketListen = INVALID_SOCKET;
  m_SocketClient = INVALID_SOCKET;

  // address info for the server to listen to
  struct addrinfo* pAIResult = NULL;
  struct addrinfo AIHints;

  // Initialize Winsock
  m_iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (m_iResult != 0)
  {
    cout << "WSAStartup failed with error: " << m_iResult << endl;
    exit(1);
  }

  // set address information
  ZeroMemory(&AIHints, sizeof(AIHints));
  AIHints.ai_family = AF_INET;
  AIHints.ai_socktype = SOCK_STREAM;
  AIHints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
  AIHints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
  m_iResult = getaddrinfo(NULL, DEFAULT_PORT, &AIHints, &pAIResult);

  if (m_iResult != 0)
  {
    cout << "getaddrinfo failed with error: " << m_iResult << endl;
    WSACleanup();
    exit(1);
  }

  // Create a SOCKET for connecting to server
  m_SocketListen = socket(pAIResult->ai_family, pAIResult->ai_socktype, pAIResult->ai_protocol);

  if (m_SocketListen == INVALID_SOCKET)
  {
    cout << "socket failed with error: " << WSAGetLastError() << endl;
    freeaddrinfo(pAIResult);
    WSACleanup();
    exit(1);
  }

  // Set the mode of the socket to be nonblocking
  u_long iMode = 1;
  m_iResult = ioctlsocket(m_SocketListen, FIONBIO, &iMode);

  if (m_iResult == SOCKET_ERROR)
  {
    cout << "ioctlsocket failed with error: " << WSAGetLastError() << endl;
    closesocket(m_SocketListen);
    WSACleanup();
    exit(1);
  }

  // Setup the TCP listening socket
  m_iResult = bind( m_SocketListen, pAIResult->ai_addr, (int)pAIResult->ai_addrlen);

  if (m_iResult == SOCKET_ERROR)
  {
    cout << "bind failed with error: %" << WSAGetLastError() << endl;
    freeaddrinfo(pAIResult);
    closesocket(m_SocketListen);
    WSACleanup();
    exit(1);
  }

  // no longer need address information
  freeaddrinfo(pAIResult);

  // start listening for new clients attempting to connect
  m_iResult = listen(m_SocketListen, SOMAXCONN);

  if (m_iResult == SOCKET_ERROR)
  {
    cout << "listen failed with error: " << WSAGetLastError() << endl;
    closesocket(m_SocketListen);
    WSACleanup();
    exit(1);
  }
}

C_TCPServerNetwork::~C_TCPServerNetwork()
{

}

// accept new connections
bool C_TCPServerNetwork::acceptNewClient(unsigned int &uiID)
{
  // if client waiting, accept the connection and save the socket
  m_SocketClient = accept(m_SocketListen, NULL, NULL);

  if (m_SocketClient != INVALID_SOCKET) 
  {
    //disable nagle on the client's socket
    char value = 1;
    setsockopt( m_SocketClient, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

    // insert new client into session id table
    m_mapSeccions.insert(pair<unsigned int, SOCKET>(uiID, m_SocketClient));

    return true;
  }
  return false;
}

// receive incoming data
int C_TCPServerNetwork::receiveData(unsigned int uiClientID, char* pszRecvBuf)
{
  if( m_mapSeccions.find(uiClientID) != m_mapSeccions.end() )
  {
    SOCKET currentSocket = m_mapSeccions[uiClientID];
    m_iResult = C_TCPService::Receive(currentSocket, pszRecvBuf, C_NetPacket::MAX_PACKET_SIZE);

    if (m_iResult == 0)
    {
      cout << "Connection closed" << endl;
      closesocket(currentSocket);
    }

    return m_iResult;
  }
  return 0;
}

// send data to all clients
void C_TCPServerNetwork::sendToAll(char* pszPackets, int iTotalSize)
{
  SOCKET currentSocket;
  map<unsigned int, SOCKET>::iterator iter;
  int iSendResult;

  for (iter = m_mapSeccions.begin(); iter != m_mapSeccions.end(); iter++)
  {
    currentSocket = iter->second;
    iSendResult = C_TCPService::Send(currentSocket, pszPackets, iTotalSize);

    if (iSendResult == SOCKET_ERROR) 
    {
      cout << "send failed with error: " << WSAGetLastError() << endl;
      closesocket(currentSocket);
    }
  }
}
