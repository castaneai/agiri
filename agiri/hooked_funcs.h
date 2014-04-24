#include <WinSock2.h>

namespace agiri
{
    namespace hook
    {
        int WINAPI connect(SOCKET socket, const sockaddr* toAddress, int addressLength);
        int WINAPI WSAConnect(SOCKET socket, const sockaddr* toAddress, int addressLength,
            LPWSABUF caller, LPWSABUF callee, LPQOS sqos, LPQOS gqos);
        int WINAPI send(SOCKET socket, char* buf, int len, int flags);
    }
}