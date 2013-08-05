#include <WinSock2.h>
#include "jansson/jansson.h"
#include "WinsockFunctionTypedef.h"

static const int BUFFER_SIZE = 2048;

void receiveForever()
{
	WSADATA data;
	WSAStartup(MAKEWORD(2, 0), &data);

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	// TODO: 待ち受けポート番号を変更可能にする
	serverAddress.sin_port = htons(10800);
	serverAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress));

	while (true) {
		char receiveBuffer[BUFFER_SIZE] = { 0 };
		sockaddr clientAddress = { 0 };
		int clientAddressLength = sizeof(clientAddress);
		recvfrom(serverSocket, receiveBuffer, sizeof(receiveBuffer), 0, &clientAddress, &clientAddressLength);

		if (strcmp(receiveBuffer, "ping") == 0) {
			sendto(serverSocket, "pong", 4, 0, &clientAddress, clientAddressLength);
		}
	}
}