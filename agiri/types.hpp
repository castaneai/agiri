#pragma once
#include <cstdint>

#define NO_DEFAULT_CONSTRUCTOR(TypeName) \
	public: \
		TypeName() = delete;

const int MaxSocketsCount = 1024;

typedef uint8_t byte_t;
typedef uint32_t socket_t;

#pragma pack(1)
struct SocketInfo
{
    socket_t id;
    uint32_t host;
    uint16_t port;
};
#pragma pack()