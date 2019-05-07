// Window Socket Programming Server Code
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include<WinSock2.h>
#include<ws2tcpip.h>
#include<iostream>

// Set a port number
#define DEFAULT_PORT "9000"
#define DEFAULT_BUFLEN 512

// The #pragma comment indicates to the linker that the Ws2_32.lib file
#pragma comment (lib, "ws2_32.lib")		

using namespace std;
int main()
{
	WSADATA wsadata;
	int iResult;
	
	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);	// Initiallize and test Winsock
	if (iResult != 0)
	{
		cout << "WSAStartup failed: " << iResult << endl;
		return 1;
	}

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));		// Memory set to zero
	hints.ai_family = AF_INET;				// Address System for IPv4 family
	hints.ai_socktype = SOCK_STREAM;		// Stream Socket type
	hints.ai_protocol = IPPROTO_TCP;		// TCP Protocol 1:1 one way transimission

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo failed with error: " << iResult << endl;
		WSACleanup();
		return 1;
	}

	// Create a ListenSocket for client connections
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "bind failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Listen from Socket
	if (listen(ListenSocket, 3) == SOCKET_ERROR)
	{
		cout << "Listen failed with error: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Wating for Connection.. " << endl;

	// Accept connection from Client
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(ListenSocket, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET)
	{
		cout << "accept faild: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)& client, sizeof(client), host, NI_MAXHOST,
		service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << "connected on port " << service << endl;
	}

	//Close Listeningsocket
	closesocket(ListenSocket);


	// do while: Receving since client shuts down the connection
	char buf[DEFAULT_BUFLEN];
	int iSendResult;
	int bufLen = DEFAULT_BUFLEN;
	
	do
	{
		iResult = recv(clientSocket, buf, bufLen, 0);
		if (iResult > 0)
		{
			cout << "Bytes received: " << iResult << endl;

			// Echo the buffer back to the sender
			iSendResult = send(clientSocket, buf, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				cout << "send failed: " << WSAGetLastError() << endl;
				closesocket(clientSocket);
				WSACleanup();
				return 1;
			}
			cout << "Bytes sent: " << iSendResult << endl;
		}
		else if (iResult == 0)
			cout << "Connection closing...\n";
		else
		{
			cout << "recv failed: " << WSAGetLastError() << endl;
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);

	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "shutdown failed: " << WSAGetLastError() << endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	// cleanup
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}