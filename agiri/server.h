#pragma once

#include <WinSock2.h>
#include <list>

typedef void(*ONRECV)(const SOCKET socket, const char* data, const int data_length);
typedef void(*ONERROR)(const char* message, const int error_code);
typedef int (WINAPI *SEND_FUNC)(SOCKET socket, char* sendBuffer, int sendLength, int flags);
typedef int (WINAPI *RECV_FUNC)(SOCKET socket, char* recvBuffer, int recvLength, int flags);

static enum class PacketType: unsigned char
{
    ListSocket = 0x01,
    InjectSend = 0x03
};

void receiveCompletely(RECV_FUNC originalRecv, SOCKET sock, const int length, char* outputBuffer)
{
    int receivedLength = 0;
    while (receivedLength < length) {
        originalRecv(sock, outputBuffer + receivedLength, length - receivedLength, 0);
    }
}

char receiveChar(RECV_FUNC originalRecv, SOCKET sock)
{
    char result;
    receiveCompletely(originalRecv, sock, sizeof(char), &result);
    return result;
}

int receiveInt32(RECV_FUNC originalRecv, SOCKET sock)
{
    int result;
    receiveCompletely(originalRecv, sock, 4, reinterpret_cast<char*>(&result));
    return result;
}

/**
 * 相手ソケットから1つのパケットを受信する
 * パケットの種類を特定し，返り値とする
 * パケットの中身は引数packetDataに入る
 * 中身の大きさはpacketDataLengthに入る
 */
PacketType receivePacket(RECV_FUNC originalRecv, SOCKET sock, __out char* packetData, __out int* packetDataLength)
{
    // 何をするパケットなのかを読み取る
    PacketType packetType = (PacketType) receiveChar(originalRecv, sock);

    // データ長を読み取る
    *packetDataLength = receiveInt32(originalRecv, sock);

    // パケットデータを読み取る
    receiveCompletely(originalRecv, sock, *packetDataLength, packetData);
}

void startServer(const char* ipAddress, const unsigned short port,
    ONRECV on_recv, ONERROR on_error, RECV_FUNC originalRecv)
{
    // winsock 初期化
    WSADATA d;
    WSAStartup(MAKEWORD(2, 0), &d);

    // サーバー用ソケット作成
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.S_un.S_addr = inet_addr(ipAddress);
    server_address.sin_port = htons(port);
    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) != 0) {
        on_error("socket bind failed", GetLastError());
        return;
    }
    if (listen(server_socket, 5) != 0) {
        on_error("socket listen failed", GetLastError());
    }
    // ノンブロッキングにする
    u_long socket_mode = 1;
    ioctlsocket(server_socket, FIONBIO, &socket_mode);

    // クライアントリスト
    std::list<SOCKET> clients;

    while (true) {
        // 新しいクライアントの受け入れ
        sockaddr_in client_address = { 0 };
        int client_address_length = sizeof(client_address);
        SOCKET client = accept(server_socket, (sockaddr*)&client_address, &client_address_length);
        if (client != INVALID_SOCKET) {
            clients.push_back(client);
        }

        // 接続済のクライアントからの受信
        for (SOCKET client : clients) {
            char buffer[0xffff] = { 0 };
            int received_length = originalRecv(client, buffer, 0xffff, 0);
            if (received_length < 1 && GetLastError() != WSAEWOULDBLOCK) {
                clients.remove(client);
                break;
            }
            if (received_length > 0) {
                PacketType packetType = receivePacket()
            }
        }
        Sleep(10);
    }

    WSACleanup();
}