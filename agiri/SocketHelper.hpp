/**
 * Winsock‚ÉŠÖ‚·‚é•â•ŠÖ”‚ğ‚Ü‚Æ‚ß‚½static class
 */
#pragma once
#include "types.hpp"

class SocketHelper final
{
public:
    static bool isSocket(const uint32_t& handle);
    static bool isTcpSocket(const socket_t& sock);
    static bool getPeerEndPoint(const socket_t& sock, uint32_t& ipAddress, uint16_t& port);
    static int getAllSockets(SocketInfo sockets[]);
private:
    SocketHelper() = delete;
    ~SocketHelper() = delete;
};