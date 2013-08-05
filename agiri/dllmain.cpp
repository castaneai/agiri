#pragma region リンカ設定

#ifdef DEBUG
#pragma comment(lib, "NCodeHook/distorm_dbg.lib")
#else
#pragma comment(lib, "NCodeHook/distorm.lib")
#endif

#pragma comment(lib, "jansson/jansson.lib")

#pragma comment(lib, "ws2_32.lib")

#pragma endregion

#pragma region ヘッダのインクルード

// Windows.hとWinsock2.hの両方をインクルードした時の定義衝突を回避する不思議の呪文
#define WIN32_LEAN_AND_MEAN

// NCodeHookの中でWindows.hも同時にインクルードされる
#include "NCodeHook/NCodeHookInstantiation.h"

#include "WinsockFunctionTypedef.h"
#include "Server.h"
#include "SimpleThread.h"
#include "TcpSocketList.h"

#pragma endregion

#pragma region エクスポート関数

// NOTE: every injected dll has to export at least one symbol - otherwise
// the OS loader will fail with STATUS_INVALID_IMAGE_FORMAT (0x0C000007B)
__declspec(dllexport) void dummyExport() {}

#pragma endregion

#pragma region フック関数

/**
 * 現在接続中のすべてのソケットを保持するリスト
 */
TcpSocketList socketList;

#pragma region connectのフック

CONNECT_FUNC originalConnect = nullptr;
int WINAPI hookedConnect(SOCKET socket, const sockaddr* toAddress, int addressLength)
{
	int result = originalConnect(socket, toAddress, addressLength);
	if (result == 0) {
		socketList.add(socket);
	}
	return result;
}

#pragma endregion

#pragma region WSAConnectのフック

WSA_CONNECT_FUNC originalWSAConnect = nullptr;
int WINAPI hookedWSAConnect(SOCKET socket, const sockaddr* toAddress, int addressLength,
	LPWSABUF caller, LPWSABUF callee, LPQOS sqos, LPQOS gqos)
{
	int result = originalWSAConnect(socket, toAddress, addressLength, caller, callee, sqos, gqos);
	if (result == 0) {
		socketList.add(socket);
	}
	return result;
}

#pragma endregion

#pragma region sendのフック

SEND_FUNC originalSend = nullptr;

int WINAPI hookedSend(SOCKET socket, char* buf, int len, int flags)
{
	return originalSend(socket, buf, len, flags);
}

#pragma endregion

#pragma endregion

#pragma region フックの実行

// hookは必ずグローバル変数にすること
NCodeHookIA32 hook;

void createHooks()
{
	originalWSAConnect = hook.createHookByName("ws2_32.dll", "WSAConnect", hookedWSAConnect);
	originalConnect = hook.createHookByName("ws2_32.dll", "connect", hookedConnect);
	originalSend = hook.createHookByName("ws2_32.dll", "send", hookedSend);
}

#pragma endregion

#pragma region フック後の処理

// serverThreadは必ずグローバル変数にすること
SimpleThread* serverThread = nullptr;
void afterHook()
{
	// TODO: ここにスレッド起動の処理を入れる
	serverThread = new SimpleThread(receiveForever);
	serverThread->Start();
}

#pragma endregion

#pragma region メイン関数

BOOL APIENTRY DllMain(HMODULE module, DWORD  reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		createHooks();
		afterHook();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#pragma endregion