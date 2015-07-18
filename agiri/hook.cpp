#include "precompiled_header.hpp"
#include "global.hpp"
#include "NCodeHook/NCodeHookInstantiation.h"

namespace hooked
{
	int WINAPI connect(SOCKET socket, const sockaddr* address, int addressLength)
	{
		return global::original_api::connect(socket, address, addressLength);
	}

	int WINAPI send(SOCKET socket, char* buf, int len, int flags)
	{
		return global::original_api::send(socket, buf, len, flags);
	}

	int WINAPI recv(SOCKET socket, char* buf, int len, int flags)
	{
		auto receivedSize = global::original_api::recv(socket, buf, len, flags);
        auto search = global::incomingPacketSniffers.find(socket);
        if (search != global::incomingPacketSniffers.end()) {
            search->second->respondSniffIncomingPacket(socket, reinterpret_cast<byte_t*>(buf), receivedSize);
        }
        return receivedSize;
	}
}

namespace
{
    NCodeHookIA32 hooker;
}

void startHook()
{
	global::original_api::connect = hooker.createHookByName("ws2_32.dll", "connect", hooked::connect);
	global::original_api::send = hooker.createHookByName("ws2_32.dll", "send", hooked::send);
	global::original_api::recv = hooker.createHookByName("ws2_32.dll", "recv", hooked::recv);
}