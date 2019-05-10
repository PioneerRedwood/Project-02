// Windows socket ���α׷��� ���� �ڵ� <���� ����>
#define WIN32_LEAN_AND_MEAN

#include<winsock2.h>
#include<ws2tcpip.h>
#include<iostream>
#include<filesystem>

#define DEFAULT_PORT "9000"
#define DEFAULT_BUFLEN 512

#pragma comment (lib, "ws2_32.lib")		

using namespace std;
int main()
{
	WSADATA wsadata;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iResult != 0)
	{
		cout << "WSAStartup failed: " << iResult << endl;
		return 1;
	}

	struct addrinfo* result = NULL, hints;

	// ��� �������� ���� ����
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// ���� �ּҿ� ��Ʈ ��ȣ�� ���
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo failed with error: " << iResult << endl;
		WSACleanup();
		return 1;
	}

	// client ������ ���� ���� ����
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// TCP ��� ���� ����
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "bind failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	if (listen(ListenSocket, 3) == SOCKET_ERROR)
	{
		cout << "Listen failed with error: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Wating for Connection.. " << endl;

	// client ������ �㰡
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(ListenSocket, (sockaddr*)& client, &clientSize);
	if (clientSocket == INVALID_SOCKET)
	{
		cout << "accept faild: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// client ���� ���� ����
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

	closesocket(ListenSocket);
	
	
	/*
	// client�� ���� ������ ���۹ޱ�
	char buf[DEFAULT_BUFLEN];
	int iSendResult;
	int bufLen = DEFAULT_BUFLEN;
	
	do
	{
		iResult = recv(clientSocket, buf, bufLen, 0);
		if (iResult > 0)
		{
			cout << "Bytes received: " << iResult << endl;

			// ���� �̿��� ������
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
	*/


	// ���� ����
	int success = 0;
	do
	{
		// ������ ���� ����
		const char* fName = "Test.txt";
		char sendBuf[DEFAULT_BUFLEN];
		errno_t err;
		cout << "Server send " << fName << " to the client" << endl;

		// �б� �������� ���� ����
		FILE* fp;
		err = fopen_s(&fp, fName, "r");
		if (err == 0)
		{
			cout << "Open success: " << fName << endl;
			return 1;
		}
		else
		{
			cout << "failed opening: " << fName << endl;
		}

		ZeroMemory(sendBuf, DEFAULT_BUFLEN);
		int fBlockSize;
		
		// ���� ������ ����
		while (fBlockSize = fread(sendBuf, sizeof(char), DEFAULT_BUFLEN, fp) > 0)
		{
			if (send(clientSocket, sendBuf, fBlockSize, 0) < 0)
			{
				cout << "Error! failed to send file : " << fName << endl;
				return 1;
			}
			else if (fBlockSize)
				break;
			ZeroMemory(sendBuf, DEFAULT_BUFLEN);
		}
		cout << "Success!\n";
		success = 1;
		closesocket(clientSocket);
		cout << "Connection closing.. \n";
		fclose(fp);
	} while (success > 0);

	// ���̻� ������ ���ٸ� ����
	iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "shutdown failed: " << WSAGetLastError() << endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	closesocket(clientSocket);
	WSACleanup();
	return 0;
}