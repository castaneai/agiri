#pragma region リンカ設定

#ifdef DEBUG
#pragma comment(lib, "NCodeHook/distorm_dbg.lib")
#else
#pragma comment(lib, "NCodeHook/distorm.lib")
#endif

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

#pragma endregion

#pragma region エクスポート関数

// NOTE: every injected dll has to export at least one symbol - otherwise
// the OS loader will fail with STATUS_INVALID_IMAGE_FORMAT (0x0C000007B)
__declspec(dllexport) void dummyExport() {}

#pragma endregion

#pragma region connectのフック

CONNECT_FUNC originalConnect = nullptr;
int WINAPI hookedConnect(SOCKET socket, const sockaddr* toAddress, int addressLength)
{
	const char* ipAddressText = inet_ntoa(reinterpret_cast<const sockaddr_in*>(toAddress)->sin_addr);
	return originalConnect(socket, toAddress, addressLength);
}

#pragma endregion

#pragma region フックの実行

// hookは必ずグローバル変数にすること
NCodeHookIA32 hook;

void createHooks()
{
	originalConnect = hook.createHookByName("ws2_32.dll", "connect", hookedConnect);
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