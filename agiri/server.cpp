#include "global.h"
#include "server.h"
#include <list>
#include "SimpleThread.h"

// server.cppの内部だけで使うグローバル変数
// 無名名前空間にすると他のファイルからアクセスされなくなる
namespace {
	// サーバーの待ち受けソケット
	SOCKET listenSocket;

	// クライアントリスト
	std::list<SOCKET> clients;
}

/**
 * 指定したソケットをノンブロッキングにする
 */
void toNonBlocking(SOCKET socket)
{
	// ノンブロッキングにする
	u_long socketMode = 1;
	ioctlsocket(socket, FIONBIO, &socketMode);
}

/**
 * 1クライアントを処理する
 *
 * クライアントが切断したりなどのエラーがあればfalseを返す
 */
bool processClient(SOCKET client)
{
	char buffer[0xffff] = { 0 };
	int receivedLength = global::original_api::recv(client, buffer, 0xffff, 0);
	// クライアントソケットはノンブロッキングモードになっているので，
	// WSAEWOULDBLOCKの場合はエラーではない．
	if (receivedLength < 1 && GetLastError() != WSAEWOULDBLOCK) {
		return false;
	}

	if (global::targetSocket != INVALID_SOCKET) {
		// クライアントから送られてきたものをそのままsendする
		global::original_api::send(global::targetSocket, buffer, receivedLength, 0);
	}
	return true;
}

/**
 * 外部クライアント待ち受けサーバーを作る
 *
 * @param listenIpAddress 待ち受けIPアドレス（ipv4文字列)
 * @param listenPort 待ち受けポート番号
 * @param onError 失敗時に呼び出すコールバック
 * @return 待ち受け(listen)するソケット
 */
SOCKET prepareServer(const char* listenIpAddress, const unsigned short listenPort, ErrorCallbackFunc onError)
{
	// winsock 初期化
	WSADATA d;
	WSAStartup(MAKEWORD(2, 0), &d);

	// サーバー用ソケット作成
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.S_un.S_addr = inet_addr(listenIpAddress);
	server_address.sin_port = htons(listenPort);
	if (bind(listenSocket, (sockaddr*) &server_address, sizeof(server_address)) != 0) {
		onError("socket bind failed", GetLastError());
		return INVALID_SOCKET;
	}
	if (listen(listenSocket, 5) != 0) {
		onError("socket listen failed", GetLastError());
		return INVALID_SOCKET;
	}
	toNonBlocking(listenSocket);

	return listenSocket;
}

/**
 * 別スレッドで実行されるサーバー関数
 */
void serverFunc()
{
	while (true) {
		// 新しいクライアントを歓迎する
		sockaddr_in clientAddress = { 0 };
		int clientAddressLength = sizeof(clientAddress);
		SOCKET newClient = accept(listenSocket, (sockaddr*) &clientAddress, &clientAddressLength);
		if (newClient != INVALID_SOCKET) {
			toNonBlocking(newClient);
			clients.push_back(newClient);
		}

		// クライアントからデータ受け取る
		for (SOCKET client : clients) {
			if (processClient(client) == false) {
				clients.remove(client);
				break;
			}
		}

		// 少しスリープしておく
		Sleep(10);
	}
}

/**
 * サーバーを始める
 */
void startServer(const char* ipAddress, const unsigned short port, ErrorCallbackFunc onError)
{
	// サーバーの用意する
	listenSocket = prepareServer(ipAddress, port, onError);

	// サーバーを別スレッドで開始
	// ここの処理はdllmainの中なので別スレッドにしないとdllのattachで対象プロセスが止まってしまうため．
	// このSimpleThreadインスタンスはdllをdetachするまで残しておくこと．でないとaccess violationが出るよ．
	// TODO: newしたままdeleteしていない
	(new SimpleThread(serverFunc))->Start();
}