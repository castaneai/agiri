#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <tchar.h>
#include <WinSock2.h>

using namespace std;

void wait_key()
{
	char temp;
	cin >> temp;
}

inline bool recvAll(const SOCKET sock, const int length, char* output)
{
    int receivedLength = 0;
    while (receivedLength < length) {
        int result = recv(sock, output + receivedLength, length - receivedLength, 0);
        if (result < 1) {
            return false;
        }
        receivedLength += result;
    }
    return true;
}
inline bool sendAll(const SOCKET sock, const int length, char* data)
{
    int sent = 0;
    while (sent < length) {
        int result = send(sock, data + sent, length - sent, 0);
        if (result < 1) {
            return false;
        }
        sent += result;
    }
    return true;
}

int main(void)
{
	WSADATA data;
	WSAStartup(MAKEWORD(2, 0), &data);

	// load agiri
	if (LoadLibrary(_T("agiri.dll")) == 0) {
		cout << "[Error]LoadLibrary agiri.dll [code:" << GetLastError() << "]" << endl;
		wait_key();
		return -1;
	}

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in address = { 0 };
	address.sin_family = AF_INET;
	address.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(10000);
	if (connect(s, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR) {
		cout << "[Error]connect failed." << endl;
		wait_key();
		return -1;
	}
    printf("socket connected. socket_id: %d\n", (int)s);

    while (true) {
        char mes[1024];
        cout << "send: " << endl;
        cin >> mes;
        int len = strlen(mes);
        mes[len] = '\n';
        sendAll(s, len + 1, mes);
        int recvLen = recv(s, mes, 1024, 0);
        cout << "recv: " << mes << endl;
    }
	return 0;
}