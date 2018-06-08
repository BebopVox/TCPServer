#include <Windows.h>

#include "C_TCPServer.h"

int main()
{
  C_TCPServer server;

  while (true)
  {
    server.update();
  }

  return 0;
}
