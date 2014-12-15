#include "global.h"

SOCKET listenSocket;

inline bool recvAll(const SOCKET sock, const int length, char* output)
{
	int receivedLength = 0;
	while (receivedLength < length) {
		int result = global::original_api::recv(sock, output + receivedLength, length - receivedLength, 0);
		if (result < 1) {
			return false;
		}
		receivedLength += result;
	}
	return true;
}

struct SocketInfo
{
	SOCKET id;
	UINT32 host;
	UINT16 port;
};

int getAllSockets(SocketInfo result[])
{
	int socketCount = 0;
	for (int i = 1; i <= 1024; i++) {
		int option;
		int len = sizeof(option);
		if (getsockopt((SOCKET) i, SOL_SOCKET, SO_TYPE, reinterpret_cast<char*>(&option), &len) == 0) {
			result[socketCount].id = i;
			sockaddr_in addr;
			int addrLen;
			getpeername(i, reinterpret_cast<sockaddr*>(&addr), &addrLen);
			result[socketCount].host = (UINT32)(addr.sin_addr.S_un.S_addr);
			result[socketCount].port = ntohs(addr.sin_port);
			socketCount++;
		}
	}
	return socketCount;
}

enum class MessageType: char
{
	Ping = 0x00,
	PingResponse = 0x01,
	ListSocket = 0x02,
	ListSocketResponse = 0x03,
};

void pongResponse(SOCKET sock)
{
	char pong [] = { 0x01, 0x00, 0x00, 0x00, 0x01 }; // datalen, pong
	global::original_api::send(sock, pong, sizeof(pong), 0);
}

void listSocketResponse(SOCKET sock)
{
	SocketInfo sockets[1024];
	int socketCount = getAllSockets(sockets);

	// dataLength
	int dataLength = sizeof(char) + sizeof(int) + socketCount * (sizeof(SOCKET) + sizeof(UINT32) + sizeof(UINT16));
	global::original_api::send(sock, (char*) &dataLength, sizeof(int), 0);

	// action
	MessageType listSocketResponse = MessageType::ListSocketResponse;
	global::original_api::send(sock, (char*) &listSocketResponse, sizeof(char), 0);

	// socketCount
	global::original_api::send(sock, (char*) &socketCount, sizeof(int), 0);

	// sockets...
	for (int i = 0; i < socketCount++; i++) {
		global::original_api::send(sock, (char*) &(sockets[i].id), sizeof(SOCKET), 0);
		global::original_api::send(sock, (char*) &(sockets[i].host), sizeof(UINT32), 0);
		global::original_api::send(sock, (char*) &(sockets[i].port), sizeof(UINT16), 0);
	}
}

void processClient(const SOCKET sock)
{
	while (true) {
		int dataLength = 0;
		// recv len
		if (!recvAll(sock, sizeof(int), reinterpret_cast<char*>(&dataLength))) {
			break;
		}

		// recv data
		char data[0xffff] = { 0 };
		if (!recvAll(sock, dataLength, data)) {
			break;
		}
		switch ((MessageType)data[0]) {

		case MessageType::Ping: 
			pongResponse(sock);
			break;

		case MessageType::ListSocket:
			listSocketResponse(sock);
			break;
		}
	}
}

DWORD WINAPI threadFunc(LPVOID)
{
	while (true) {
		sockaddr_in clientAddress = { 0 };
		int clientAddressLength = sizeof(clientAddress);
		SOCKET newClient = accept(listenSocket, (sockaddr*) &clientAddress, &clientAddressLength);
		if (newClient != INVALID_SOCKET) {
			processClient(newClient);
		}
	}
}

SOCKET prepareServer(const char* listenIpAddress, const unsigned short listenPort)
{
	// winsock 初期化
	// winsockを使用するプロセスが対象のはずだから，わざわざ初期化する必要なしと思うが，
	// Injectのタイミングがすっごい早かったら必要かもしれないし，一応初期化はしておこう
	WSADATA d;
	WSAStartup(MAKEWORD(2, 0), &d);

	// サーバー用ソケット作成
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.S_un.S_addr = inet_addr(listenIpAddress);
	server_address.sin_port = htons(listenPort);
	bind(sock, (sockaddr*) &server_address, sizeof(server_address));
	listen(sock, 5);

	return sock;
}

void StartServer(const char* host, const unsigned short port)
{
	listenSocket = prepareServer(host, port);
	// start server on new-thread.
	QueueUserWorkItem(threadFunc, nullptr, 0);
}