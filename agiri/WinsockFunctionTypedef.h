#pragma once
#include <WinSock2.h>
typedef int (WINAPI *CONNECT_FUNC)(SOCKET socket, const sockaddr* toAddress, int addressLength);
typedef int (WINAPI *SEND_FUNC)(SOCKET socket, char* sendBuffer, int sendLength, int flags);
typedef int (WINAPI *RECV_FUNC)(SOCKET socket, char* recvBuffer, int recvLength, int flags);