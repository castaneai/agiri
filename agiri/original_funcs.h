#pragma once
#include <WinSock2.h>

namespace agiri {
    namespace hook {
        namespace original {

            // フックする前の元の関数のアドレスを保存しておくポインタたち

            extern int (WINAPI *connect)(SOCKET socket, const sockaddr* toAddress, int addressLength);
            extern int (WINAPI *WSAConnect)(SOCKET socket, const sockaddr* toAddress, int addressLength,
                LPWSABUF caller, LPWSABUF callee, LPQOS sqos, LPQOS gqos);
            extern int (WINAPI *send)(SOCKET socket, char* sendBuffer, int sendLength, int flags);
            extern int (WINAPI *recv)(SOCKET socket, char* recvBuffer, int recvLength, int flags);

        }
    }
}