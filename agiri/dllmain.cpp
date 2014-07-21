#pragma region リンカ設定

#ifdef DEBUG
#pragma comment(lib, "NCodeHook/distorm_dbg.lib")
#else
#pragma comment(lib, "NCodeHook/distorm.lib")
#endif

#pragma comment(lib, "ws2_32.lib")

#pragma endregion

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "global.h"
#include "hook.h"
#include "server.h"

#pragma region エクスポート関数

// NOTE: every injected dll has to export at least one symbol - otherwise
// the OS loader will fail with STATUS_INVALID_IMAGE_FORMAT (0x0C000007B)
__declspec(dllexport) void dummyExport() {}

#pragma endregion

#pragma region メイン関数

// サーバーのbindやlistenで失敗した時に呼び出される
// ログ記録などが望ましいが，面倒なので今はmessageboxで
void onError(const char* message, int errorCode)
{
	MessageBoxA(nullptr, message, "error!", MB_OK);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD  reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
        startHook();
		// TODO: IPとポート番号を変更可能にする
        startServer("127.0.0.1", 10800, onError);
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#pragma endregion