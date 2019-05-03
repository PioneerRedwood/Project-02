// Window Socket Programming Server Code
#include<WinSock2.h>
#include<iostream>
#pragma comment (lib, "ws2_32.lib")		
// The #pragma comment indicates to the linker that the Ws2_32.lib file is needed.
using namespace std;
int main()
{
	WSADATA wsadata;						// Create New Winsock
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);	// Initiallize and test Winsock
	if (iResult != 0)
	{
		cout << "WSAStartup failed: " << iResult << endl;
		return 1;
	}

	WSACleanup();							// Destroy Winsock
}