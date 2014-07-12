#pragma once

#include <WinSock2.h>

typedef void(*ONRECV)(const SOCKET socket, const char* data, const int data_length);
typedef void(*ONERROR)(const char* message, const int error_code);
typedef int (WINAPI *SEND_FUNC)(SOCKET socket, char* sendBuffer, int sendLength, int flags);
typedef int (WINAPI *RECV_FUNC)(SOCKET socket, char* recvBuffer, int recvLength, int flags);

void startServer(const char* ipAddress, const unsigned short port,
    ONRECV on_recv, ONERROR on_error, RECV_FUNC originalRecv);