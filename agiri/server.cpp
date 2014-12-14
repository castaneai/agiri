#include "global.h"

SOCKET listenSocket;

inline void recvAll(const SOCKET sock, const int length, char* output)
{
	int receivedLength = 0;
	while (receivedLength < length) {
		receivedLength += global::original_api::recv(sock, output + receivedLength, length - receivedLength, 0);
	}
}

void processClient(const SOCKET sock)
{
	while (true) {
		int dataLength = 0;
		// recv len
		recvAll(sock, sizeof(short), reinterpret_cast<char*>(&dataLength));

		// recv data
		char data[0xffff] = { 0 };
		recvAll(sock, dataLength, data);
		switch (data[0]) {
		// ping
		case 0x00: 
			MessageBox(nullptr, L"recv ping!", L"", 0);
			char response[] = { 0x01, 0x00, 0x01 }; // datalen, pong
			global::original_api::send(sock, response, 1, 0);
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