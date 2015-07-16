#include <cstdint>
#include "global.h"

const int maxSocketCount = 1024;

enum class Command : uint8_t
{
    PingRequest = 0x00,
    PongResponse = 0x01,
    ListSocketRequest = 0x02,
    ListSocketResponse = 0x03,
};

#pragma pack(1)
struct Request
{
    Command command;
    uint32_t dataLength;
    uint8_t data[0xffff];
};

struct SocketInfo
{
    uint32_t id;
    uint32_t host;
    uint16_t port;
};

struct ListSocketResponse
{
    uint32_t socketCount;
    SocketInfo sockets[maxSocketCount];

    uint32_t getSize() const
    {
        return sizeof(uint32_t) + sizeof(SocketInfo) * socketCount;
    }
};
#pragma pack()


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

inline bool sendAll(const SOCKET sock, const int length, char* data)
{
    int sent = 0;
    while (sent < length) {
        int result = global::original_api::send(sock, data + sent, length - sent, 0);
        if (result < 1) {
            return false;
        }
        sent += result;
    }
    return true;
}

int getAllSockets(SocketInfo result[])
{
	int socketCount = 0;
	for (int i = 1; i <= maxSocketCount; i++) {
        int socketType;
        int len = sizeof(socketType);
        if (GetFileType(reinterpret_cast<HANDLE>(i)) != FILE_TYPE_PIPE ||
            getsockopt((SOCKET)i, SOL_SOCKET, SO_TYPE, reinterpret_cast<char*>(&socketType), &len) == SOCKET_ERROR) {
            continue;
        }
        if (socketType != SOCK_STREAM) {
            continue;
        }

        sockaddr_in addr;
        int addrLen = sizeof(addr);
        if (GetFileType((HANDLE) i) == FILE_TYPE_PIPE && getpeername((SOCKET) i, reinterpret_cast<sockaddr*>(&addr), &addrLen) != SOCKET_ERROR) {
            result[socketCount].id = i;
            result[socketCount].host = (uint32_t)(addr.sin_addr.S_un.S_addr);
            result[socketCount].port = ntohs(addr.sin_port);
            socketCount++;
        }
	}
	return socketCount;
}

void sendResponse(const SOCKET sock, Command command, char* data, uint32_t dataLength)
{
    sendAll(sock, sizeof(command), reinterpret_cast<char*>(&command));
    sendAll(sock, sizeof(dataLength), reinterpret_cast<char*>(&dataLength));
    sendAll(sock, dataLength, data);
}

void pongResponse(SOCKET sock)
{
    sendResponse(sock, Command::PongResponse, nullptr, 0);
}

void listSocketResponse(SOCKET sock)
{
    ListSocketResponse response;
	response.socketCount = getAllSockets(response.sockets);
    sendResponse(sock, Command::ListSocketResponse, (char*)&response, response.getSize());
}

bool receiveRequest(const SOCKET sock, Request& request)
{
    if (!recvAll(sock, sizeof(request.command), reinterpret_cast<char*>(&request.command))) {
        return false;
    }
    if (!recvAll(sock, sizeof(request.dataLength), reinterpret_cast<char*>(&request.dataLength))) {
        return false;
    }
    if (!recvAll(sock, request.dataLength, reinterpret_cast<char*>(request.data))) {
        return false;
    }
    return true;
}

void processClient(const SOCKET sock)
{
	while (true) {
        Request request;
        if (!receiveRequest(sock, request)) {
            break;
        }
		switch (request.command) {
        case Command::PingRequest: 
			pongResponse(sock);
			break;

        case Command::ListSocketRequest:
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