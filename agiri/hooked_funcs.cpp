#include <WinSock2.h>
#include "original_funcs.h"

namespace agiri {

    namespace hook {

        int WINAPI connect(SOCKET socket, const sockaddr* toAddress, int addressLength)
        {
            int result = original::connect(socket, toAddress, addressLength);
            if (result == 0) {
                // socketList.add(socket);
            }
            return result;
        }

        int WINAPI WSAConnect(SOCKET socket, const sockaddr* toAddress, int addressLength,
            LPWSABUF caller, LPWSABUF callee, LPQOS sqos, LPQOS gqos)
        {
            int result = original::WSAConnect(socket, toAddress, addressLength, caller, callee, sqos, gqos);
            if (result == 0) {
                // socketList.add(socket);
            }
            return result;
        }

        int WINAPI send(SOCKET socket, char* buf, int len, int flags)
        {
            return original::send(socket, buf, len, flags);
        }

    }

}