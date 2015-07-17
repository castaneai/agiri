#include "precompiled_header.hpp"
#include "types.hpp"

void startHook();
void startServer(const char* const& ipAddress, const uint16_t& port);

BOOL APIENTRY DllMain(HMODULE, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH) {
		startHook();
		startServer("127.0.0.1", 10800);
	}
	return TRUE;
}