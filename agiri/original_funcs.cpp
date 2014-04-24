#include "original_funcs.h"

namespace agiri
{
    namespace hook
    {
        namespace original
        {
            int (WINAPI *connect)(SOCKET socket, const sockaddr* toAddress, int addressLength) = nullptr;
            int (WINAPI *WSAConnect)(SOCKET socket, const sockaddr* toAddress, int addressLength,
                LPWSABUF caller, LPWSABUF callee, LPQOS sqos, LPQOS gqos) = nullptr;
            int (WINAPI *send)(SOCKET socket, char* sendBuffer, int sendLength, int flags) = nullptr;
            int (WINAPI *recv)(SOCKET socket, char* recvBuffer, int recvLength, int flags) = nullptr;
        }
    }
}