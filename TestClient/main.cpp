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
        Sleep(1000);
    }
	return 0;
}