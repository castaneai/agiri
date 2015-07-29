#pragma once
#include <cstdint>

#define NO_DEFAULT_CONSTRUCTOR(TypeName) \
	public: \
		TypeName() = delete;

const uint32_t MaxSocketsCount = 0xffff;

typedef uint8_t byte_t;
typedef uint32_t socket_t;

#ifdef _UNICODE
typedef std::wstring string_t;
#else
typedef std::string string_t;
#endif

#pragma pack(1)
struct SocketInfo
{
    socket_t id;
    uint32_t host;
    uint16_t port;
};
#pragma pack()