// Windows.hとWinsock2.hの両方をインクルードした時の定義衝突を回避する不思議の呪文
#define WIN32_LEAN_AND_MEAN

// WinAPIフックのライブラリ NCodeHook
// NCodeHookの中でWindows.hも同時にインクルードされる
#include "NCodeHook/NCodeHookInstantiation.h"

#include <WinSock2.h>

#include "agiri.h"
#include "SimpleThread.h"
#include "server.h"

namespace agiri
{
    /**
     * APIフック本体
     */
    NCodeHookIA32 hooker;

    const SimpleThread* serverThread = nullptr;

    /**
     * threadFuncで必要となるIPアドレス
     */
    const char* serverIpAddress = nullptr;

    /**
     * threadFuncで必要となるポート番号
     */
    unsigned short serverPort = 0;

#pragma region 元の関数への参照

    namespace original
    {
        int (WINAPI *connect)(SOCKET socket, const sockaddr* toAddress, int addressLength);
        int (WINAPI *WSAConnect)(SOCKET socket, const sockaddr* toAddress, int addressLength,
            LPWSABUF caller, LPWSABUF callee, LPQOS sqos, LPQOS gqos);
        int (WINAPI *send)(SOCKET socket, char* sendBuffer, int sendLength, int flags);
        int (WINAPI *recv)(SOCKET socket, char* recvBuffer, int recvLength, int flags);
    }

#pragma endregion

#pragma region フック関数

    namespace hooked
    {
        int WINAPI connect(SOCKET socket, const sockaddr* toAddress, int addressLength)
        {
            int result = original::connect(socket, toAddress, addressLength);
            if (result == 0) {
                // socketList.add(socket);
            }
            return result;
        }

        int WINAPI WSAConnect(SOCKET socket, const sockaddr* toAddress, int addressLength,
            LPWSABUF caller, LPWSABUF callee, LPQOS sqos, LPQOS gqos)
        {
            int result = original::WSAConnect(socket, toAddress, addressLength, caller, callee, sqos, gqos);
            if (result == 0) {
                // socketList.add(socket);
            }
            return result;
        }

        int WINAPI send(SOCKET socket, char* buf, int len, int flags)
        {
            return original::send(socket, buf, len, flags);
        }
    }

#pragma endregion

    void createHooks()
    {
        original::WSAConnect = hooker.createHookByName("ws2_32.dll", "WSAConnect", hooked::WSAConnect);
        original::connect = hooker.createHookByName("ws2_32.dll", "connect", hooked::connect);
        original::send = hooker.createHookByName("ws2_32.dll", "send", hooked::send);
    }

    void start()
    {
        createHooks();
    }

    /**
    * 別スレッドで実行する関数
    * startServerをそのままSimpleThreadに渡したかったが，
    * SimpleThreadが受け付けるのは引数なしの関数だけなので，
    * thisポインタを内部で受け取っているメンバ関数は渡せない
    */
    void threadFunc()
    {
        // サーバーを立てる
        startServer(serverIpAddress, serverPort);
    }

    /**
    * 新しいあぎりさんを作る
    *
    * @param serverIpAddress 差し込むsendを受け取るサーバーの待ち受けIPアドレス
    * @param serverPort 差し込むsendを受け取るサーバーの待受ポート番号
    */
    Agiri::Agiri(const char* serverIpAddress, const unsigned short serverPort)
    {
        agiri::serverIpAddress = serverIpAddress;
        agiri::serverPort = serverPort;
        agiri::serverThread = new SimpleThread(agiri::threadFunc);
        agiri::serverThread->Start();
    }
}