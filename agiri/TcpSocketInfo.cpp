#include "TcpSocketInfo.h"

TcpSocketInfo::TcpSocketInfo(SOCKET source)
{
	this->socketId = (int) source;
}

const int TcpSocketInfo::getSocketId() const
{
	return this->socketId;
}

const char* TcpSocketInfo::getAddress() const
{
	sockaddr_storage address;
	int addressLength = sizeof(address);
	getpeername(this->socketId, reinterpret_cast<sockaddr*>(&address), &addressLength);
	return inet_ntoa(reinterpret_cast<const sockaddr_in*>(&address)->sin_addr);
}

const unsigned short TcpSocketInfo::getPort() const
{
	sockaddr_storage address;
	int addressLength = sizeof(address);
	getpeername(this->socketId, reinterpret_cast<sockaddr*>(&address), &addressLength);
	return ntohs(reinterpret_cast<const sockaddr_in*>(&address)->sin_port);
}