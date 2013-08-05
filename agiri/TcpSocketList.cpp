#include "TcpSocketList.h"

void TcpSocketList::add(SOCKET socket)
{
	if (this->has(socket) == false) {
		TcpSocketInfo info(socket);
		this->list.push_back(info);
	}
}

bool TcpSocketList::has(SOCKET socket) const
{
	for (auto info : this->list) {
		if (info.getSocketId() == (int) socket) return true;
	}
	return false;
}

void TcpSocketList::remove(SOCKET socket)
{
	if (this->has(socket)) {
		this->list.remove_if([&socket](const TcpSocketInfo& info) {
			return info.getSocketId() == (int) socket;
		});
	}
}