#include "Agiri.h"
#include "MiniServer.h"
#include "SimpleThread.h"
#include "original_funcs.h"
#include "hooked_funcs.h"

namespace agiri
{

    /**
     * threadFuncで必要となるIPアドレス
     * threadFuncはstatic関数なので，そこからアクセスできるようにするには
     * このように変数自体もstaticにしておくしかない．
     * できればAgiriクラスのコンストラクタから渡したかったが・・
     */
    static const char* serverIpAddress = nullptr;

    /**
     * threadFuncで必要となるポート番号
     * IPアドレスと同じ理由によりstaticである
     */
    static unsigned short serverPort = 0;

    /**
     * 別スレッドで実行する関数
     * agiri::server::MiniServer::receiveForeverをそのままSimpleThreadに渡したかったが，
     * SimpleThreadが受け付けるのは引数なしの関数だけなので，
     * thisポインタを内部で受け取っているメンバ関数は渡せない
     * だから，ここでstaticなスレッド実行用の関数を作り，
     * 中身ではサーバーを立てて永遠待ち受けするだけの処理を入れてある
     */
    static void threadFunc()
    {
        // サーバーを立てる
        server::MiniServer(serverIpAddress, serverPort).receiveForever();
    }

    /**
     * 新しいあぎりさんを作る
     *
     * @param serverIpAddress 差し込むsendを受け取るサーバーの待ち受けIPアドレス
     * @param serverPort 差し込むsendを受け取るサーバーの待受ポート番号
     */
    Agiri::Agiri(const char* serverIpAddress, const unsigned short serverPort)
        : serverThread(new SimpleThread(agiri::threadFunc)), hook()
    {
        agiri::serverIpAddress = serverIpAddress;
        agiri::serverPort = serverPort;
        this->serverThread->Start();
    }


    Agiri::~Agiri()
    {
        if (this->serverThread != nullptr) {
            delete this->serverThread;
        }
    }

    void Agiri::start()
    {
        this->createHooks();
    }


    void Agiri::createHooks()
    {
        hook::original::WSAConnect = this->hook.createHookByName("ws2_32.dll", "WSAConnect", hook::WSAConnect);
        hook::original::connect = this->hook.createHookByName("ws2_32.dll", "connect", hook::connect);
        hook::original::send = this->hook.createHookByName("ws2_32.dll", "send", hook::send);
    }
}