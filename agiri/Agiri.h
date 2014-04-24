#pragma once

#pragma region ヘッダインクルード

// Windows.hとWinsock2.hの両方をインクルードした時の定義衝突を回避する不思議の呪文
#define WIN32_LEAN_AND_MEAN

// WinAPIフックのライブラリ NCodeHook
// NCodeHookの中でWindows.hも同時にインクルードされる
#include "NCodeHook/NCodeHookInstantiation.h"

#pragma endregion

namespace agiri {

    class SimpleThread;

    class Agiri
    {
    public:
        Agiri(const char* serverIpAddress, const unsigned short serverPort);
        ~Agiri();
        void start();

    private:
        SimpleThread* serverThread;
        NCodeHookIA32 hook;

        void createHooks();
    };
}