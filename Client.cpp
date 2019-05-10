// Windows socket ���α׷��� Ŭ���̾�Ʈ �ڵ� <���� ����>
#define WIN32_LEAN_AND_MEAN

#include<iostream>
#include<string>
#include<ws2tcpip.h>
#include<winsock2.h>
#include<filesystem>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "9000"

int main()
{
	WSADATA wsaData;
	int iResult;

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult == INVALID_SOCKET)
	{
		cout << "WSAStartup failed: " << WSAGetLastError() << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// ���� �ּҿ� ��Ʈ�� ����
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo failed: " << iResult << endl;
		WSACleanup();
		return 1;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;
	ptr = result;

	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// ������ ����
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Unable to connect to server!" << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	/*
	// ������ ������ ���� ������ ������ ����
	string message;
	do
	{
		cout << ">> ";
		getline(cin, message);

		// ����ڰ� �����̵� �Է��ϸ� ����
		if (message.size() > 0)
		{
			// �޽��� ����
			iResult = send(ConnectSocket, message.c_str(), message.size() + 1, 0);
			if (iResult != SOCKET_ERROR)
			{
				// ���� ��� �� ���� �ʱ�ȭ
				ZeroMemory(recvBuf, DEFAULT_BUFLEN);
				int bytesRecv = recv(ConnectSocket, recvBuf, DEFAULT_BUFLEN, 0);
				if (bytesRecv > 0)
				{
					// ������ �ܼ�â�� ������
					cout << "Server>> " << string(recvBuf, 0, bytesRecv) << endl;
				}
			}
		}
	} while (message.size() > 0);
	*/

	int recvBufLen = DEFAULT_BUFLEN;
	char recvBuf[DEFAULT_BUFLEN];

	// ���� ����
	const char* fName = "ReceivedTest.txt";
	FILE* fp;
	errno_t err;
	err = fopen_s(&fp, fName, "a");

	if (err == 0)
	{
		cout << "Open success: " << fName << endl;
		return 1;
	}
	else
	{
		cout << "failed opening: " << fName << endl;
	}

	ZeroMemory(recvBuf, DEFAULT_BUFLEN);
	int fBlockSize = 0;
	int success = 0;
	do
	{
		while (fBlockSize = recv(ConnectSocket, recvBuf, DEFAULT_BUFLEN, 0))
		{
			if (fBlockSize < 0)
			{
				cout << "Error! Receiving file.\n";
				break;
			}
			else
			{
				int writeSize = fwrite(recvBuf, sizeof(char), fBlockSize, fp);
				if (writeSize < fBlockSize)
				{
					cout << "Error! Writing failed.\n";
					break;
				}
			}
			ZeroMemory(recvBuf, DEFAULT_BUFLEN);
		}
		cout << "Success!\n";
		success = 1;
		fclose(fp);
	} while (success == 0);

	// ���̻� ������ �����Ͱ� ���� �� ���� �ݱ�
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "shutdown failed: " << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// ����
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}