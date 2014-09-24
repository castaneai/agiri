#pragma once
#include <WinSock2.h>

using SocketReadWriteFunc = int(*WINAPI)(SOCKET sock, char* buffer, int length, int flags);

class AgiriNetworkClient
{
public:
	AgiriNetworkClient(const SOCKET agiriSocket);
	bool send(const char* sendBuffer, int sendLength) const;
	int recv(char* recvBuffer) const;

private:
	bool exactlyBase(SocketReadWriteFunc func, char* buffer, const int expectedLength) const;
	bool sendExactly(char* sendBuffer, const int sendLength) const;
	bool recvExactly(char* recvBuffer, const int recvLength) const;
	const SOCKET agiriSocket;
};
