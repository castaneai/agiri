#pragma once 
#include <list>
#include <WinSock2.h>
#include "TcpSocketInfo.h"

class TcpSocketList
{
public:
	void add(SOCKET socket);
	bool has(SOCKET socket) const;
	void remove(SOCKET socket);

private:
	std::list<TcpSocketInfo> list;
};