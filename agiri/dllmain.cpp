#pragma comment(lib, "NCodeHook/distorm.lib")
#pragma comment(lib, "ws2_32.lib")

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "global.h"

void StartHook();
void StartServer(const char* host, const unsigned short port);

BOOL APIENTRY DllMain(HMODULE, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH) {
		StartHook();
		StartServer("127.0.0.1", 10800);
	}
	return TRUE;
}