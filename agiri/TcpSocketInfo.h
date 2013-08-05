#pragma once
#include <WinSock2.h>

class TcpSocketInfo
{
public:
	TcpSocketInfo(SOCKET source);
	const int getSocketId() const;
	const char* getAddress() const;
	const unsigned short getPort() const;

private:
	int socketId;
};