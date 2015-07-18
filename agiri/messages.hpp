/*
 * injectされたagiri.dllと外部のagiri対応プログラム (mediator) がやりとりするメッセージを定義
 */
#pragma once
#include <cstdint>
#include "types.hpp"

const int MaxDataSize = 0xffff;

enum struct Command : uint8_t
{
    PingRequest = 0x00,
    PongResponse = 0x01,
    ListSocketRequest = 0x02,
    ListSocketResponse = 0x03,
    InjectOutgoingPacketRequest = 0x04,
    SwitchSniffIncomingPacketRequest = 0x05,
    IncomingPacketResponse = 0x06,
};

#pragma pack(1)
struct Message
{
    Command command;
    uint32_t dataSize;
    byte_t data[MaxDataSize];
    inline uint32_t getSize() const
    {
        return sizeof(command) + sizeof(data[0]) * dataSize;
    }
};

#pragma region Requests
struct InjectOutgoingPacketRequest
{
    socket_t targetSocket;
    uint32_t packetSize;
    byte_t packetData[MaxDataSize];
};

struct SwitchSniffIncomingPacketRequest
{
    socket_t targetSocket;
    byte_t state; // 0: OFF, 1: ON
};
#pragma endregion

#pragma region Responses
struct ListSocketResponse
{
    uint32_t socketsCount;
    SocketInfo sockets[MaxSocketsCount];

    uint32_t getSize() const
    {
        return sizeof(socketsCount) + sizeof(sockets[0]) * socketsCount;
    }
};

struct SniffIncomingPacketResponseHeader
{
    socket_t socket;
    uint32_t packetSize;
};
#pragma endregion

#pragma pack()