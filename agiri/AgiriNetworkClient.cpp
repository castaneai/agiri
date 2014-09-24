#include "global.h"
#include "AgiriNetworkClient.h"

AgiriNetworkClient::AgiriNetworkClient(const SOCKET agiriSocket)
	: agiriSocket(agiriSocket)
{
}

bool AgiriNetworkClient::send(const char* sendBuffer, int sendLength) const
{
	// write length
	if (this->sendExactly(reinterpret_cast<char*>(sendLength), sizeof(DWORD) == false)) {
		return false;
	}
	// write data
	if (this->sendExactly(const_cast<char*>(sendBuffer), sendLength) == false) {
		return false;
	}
	return true;
}

int AgiriNetworkClient::recv(char* recvBuffer) const
{
	// read length

	return 0;
}

bool AgiriNetworkClient::exactlyBase(SocketReadWriteFunc func, char * buffer, const int expectedLength) const
{
	int finishedLength = 0;
	while (finishedLength < expectedLength) {
		int result = func(this->agiriSocket, buffer + finishedLength, expectedLength - finishedLength, 0);
		if (result == SOCKET_ERROR) {
			return false;
		}
		finishedLength += result;
	}
	return true;
}

bool AgiriNetworkClient::sendExactly(char* sendBuffer, const int sendLength) const
{
	return this->exactlyBase(
		reinterpret_cast<SocketReadWriteFunc>(global::original_api::send),
		sendBuffer,
		sendLength);
}

bool AgiriNetworkClient::recvExactly(char* recvBuffer, const int recvLength) const
{
	return this->exactlyBase(
		reinterpret_cast<SocketReadWriteFunc>(global::original_api::recv),
		recvBuffer,
		recvLength);
}
