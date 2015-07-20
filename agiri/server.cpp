#include "precompiled_header.hpp"
#include "NinjaConnection.hpp"
#include "RequestHandler.hpp"

namespace
{
    SOCKET listenSocket;

    DWORD WINAPI onAcceptSnifferThreadFunc(void* argument)
    {
        auto snifferSock = (reinterpret_cast<const socket_t>(argument));
        NinjaConnection conn(snifferSock);
        RequestHandler handler(conn);
        while (true) {
            Message mes;
            if (!conn.receive(mes)) {
                break;
            }
            handler.handle(mes);
        }
        return 0;
    }

    DWORD WINAPI acceptSnifferThreadFunc(LPVOID)
    {
        while (true) {
            sockaddr_in snifferAddress = { 0 };
            int snifferAddressLength = sizeof(snifferAddress);
            auto acceptedSnifferSock = accept(listenSocket, reinterpret_cast<sockaddr*>(&snifferAddress), &snifferAddressLength);
            if (acceptedSnifferSock != INVALID_SOCKET) {
                QueueUserWorkItem(onAcceptSnifferThreadFunc, reinterpret_cast<PVOID>(acceptedSnifferSock), 0);
            }
        }
    }

    socket_t createListenSocket(const char* const& listenIPAddress, const unsigned short& listenPort)
    {
        auto sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockaddr_in listenAddress;
        listenAddress.sin_family = AF_INET;
        listenAddress.sin_addr.S_un.S_addr = inet_addr(listenIPAddress);
        listenAddress.sin_port = htons(listenPort);
        bind(sock, reinterpret_cast<sockaddr*>(&listenAddress), sizeof(listenAddress));
        listen(sock, 5);
        return sock;
    }
}

void startServer(const char* const& listenIPAddress, const unsigned short& listenPort)
{
    // winsock 初期化
    // winsockを使用するプロセスが対象のはずだから，わざわざ初期化する必要なしと思うが，
    // Injectのタイミングがすっごい早かったら必要かもしれないし，一応初期化はしておこう
    WSADATA d;
    WSAStartup(MAKEWORD(2, 0), &d);

    listenSocket = createListenSocket(listenIPAddress, listenPort);
    QueueUserWorkItem(acceptSnifferThreadFunc, nullptr, 0);
}