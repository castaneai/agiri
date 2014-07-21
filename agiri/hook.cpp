#include "global.h"
#include "hook.h"
#include <WinSock2.h>

// WinAPIフックのライブラリ NCodeHook
// NCodeHookの中でWindows.hも同時にインクルードされる
#include "NCodeHook/NCodeHookInstantiation.h"

namespace {
	/**
	 * APIフック本体
	 */
	NCodeHookIA32 hooker;
}

#pragma region フック関数

namespace hooked
{
	int WINAPI connect(SOCKET socket, const sockaddr* address, int addressLength)
	{
		int result = global::original_api::connect(socket, address, addressLength);
		if (result == 0) {
			global::targetSocket = socket;
		}
		return result;
	}

	int WINAPI send(SOCKET socket, char* buf, int len, int flags)
	{
		if (global::targetSocket == INVALID_SOCKET) {
			global::targetSocket = socket;
		}
		return global::original_api::send(socket, buf, len, flags);
	}

	int WINAPI recv(SOCKET socket, char* buf, int len, int flags)
	{
		return global::original_api::recv(socket, buf, len, flags);
	}
}

#pragma endregion

void startHook()
{
	global::original_api::connect = hooker.createHookByName("ws2_32.dll", "connect", hooked::connect);
	global::original_api::send = hooker.createHookByName("ws2_32.dll", "send", hooked::send);
	global::original_api::recv = hooker.createHookByName("ws2_32.dll", "recv", hooked::recv);
}