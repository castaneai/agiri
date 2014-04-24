#include "MiniServer.h"
#include <WinSock2.h>

static const int BUFFER_SIZE = 2048;

agiri::server::MiniServer::MiniServer(const char* ipAddress, const unsigned short port) :
ipAddress(ipAddress), port(port)
{
}

void agiri::server::MiniServer::receiveForever() const
{
	WSADATA data;
	WSAStartup(MAKEWORD(2, 0), &data);

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(this->port);
	serverAddress.sin_addr.S_un.S_addr = inet_addr(this->ipAddress);

	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress));

	while (true) {
		char receiveBuffer[BUFFER_SIZE] = { 0 };
		sockaddr clientAddress = { 0 };
		int clientAddressLength = sizeof(clientAddress);
		recvfrom(serverSocket, receiveBuffer, sizeof(receiveBuffer), 0, &clientAddress, &clientAddressLength);

        // TODO: 各種メッセージを受け取り解釈する
		if (strcmp(receiveBuffer, "ping") == 0) {
			sendto(serverSocket, "pong", 4, 0, &clientAddress, clientAddressLength);
		}
	}
}