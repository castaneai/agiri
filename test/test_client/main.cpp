#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <WinSock2.h>

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("Usage: TestClient <connect_port>\n");
        return 0;
    }
    int connectPort = atoi(argv[1]);

    WSADATA data;
    WSAStartup(MAKEWORD(2, 0), &data);

    if (LoadLibraryA("agiri.dll") == 0) {
        printf("[ERROR!]LoadLibrary agiri.dll [code:%d]\n", GetLastError());
        return -1;
    }

    while (true) {
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockaddr_in address = { 0 };
        address.sin_family = AF_INET;
        address.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
        address.sin_port = htons(connectPort);
        if (connect(s, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR) {
            printf("[ERROR!]connect failed.\n");
            return -1;
        }
        printf("test connection (handle: %d) established.\n", s);

        while (true) {
            char recvBuf[1024] = { 0 };
            int recvLen = recv(s, recvBuf, 1024, 0);
            if (recvLen <= 0) {
                printf("test connection (handle: %d) closed.\n", s);
                break;
            }
            if (recvLen > 0) {
                printf("recv: %s\n", recvBuf);
            }
        }
    }
    return 0;
}