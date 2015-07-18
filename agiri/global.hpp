// 複数のファイルから共用するグローバル変数を定義する
// グローバル変数はglobalという名前空間に閉じ込め，名前汚染しないようにする
// 
// 本来ヘッダでグローバル変数は定義できないが，Microsoft独自のselectanyにより
// リンカに1度だけ定義を認識させることにより，二重定義のエラーを回避しグローバル変数を定義している
// 詳細は http://msdn.microsoft.com/ja-jp/library/5tkz6s71.aspx

#pragma once
#include "precompiled_header.hpp"
#include "RequestHandler.hpp"

// C++11からusingをtypedefの代わりに使えるようになっている
// 対応はVS2013から？
using RecvFunc = int (WINAPI*) (SOCKET socket, char* recvBuffer, int recvLength, int flags);
using SendFunc = int (WINAPI*) (SOCKET socket, char* sendBuffer, int sendLength, int flags);
using ConnectFunc = int (WINAPI*) (SOCKET socket, const sockaddr* address, int addressLength);

namespace global {
	// フックしたWinsockの元の関数
	namespace original_api {
		__declspec(selectany) RecvFunc recv;
		__declspec(selectany) SendFunc send;
		__declspec(selectany) ConnectFunc connect;
	}

    // このプロセスがrecvしたパケットを外部のsnifferに流すかどうか
    // このセットにソケットID-keyが含まれている場合はvalueのRequestHandlerに流す
    __declspec(selectany) std::unordered_map<socket_t, const RequestHandler*> incomingPacketSniffers;
}