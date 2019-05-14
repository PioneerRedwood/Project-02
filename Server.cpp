// Windows socket 프로그래밍 서버 코드 <파일 전송>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<iostream>
#include<filesystem>
#include<fstream>

#define DEFAULT_PORT "9000"
#define DEFAULT_BUFLEN 4096

#pragma comment (lib, "ws2_32.lib")		

using namespace std;
int main()
{
	WSADATA wsadata;
	int iResult;

	char buf[DEFAULT_BUFLEN];
	int bufLen = DEFAULT_BUFLEN;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iResult != 0)
	{
		cout << "WSAStartup failed: " << iResult << endl;
		return 1;
	}

	struct addrinfo* result = NULL, hints;

	// 통신 프로토콜 정보 설정
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// 서버 주소와 포트 번호를 등록
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo failed with error: " << iResult << endl;
		WSACleanup();
		return 1;
	}

	// client 접속을 받을 소켓 생성
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// TCP 통신 소켓 설정
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

	// client 접속을 허가
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

	// client 접속 내용 전시
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

	iResult = recv(clientSocket, buf, bufLen, 0);
	if (iResult == SOCKET_ERROR)
	{
		cout << "failed recv SYN: " << WSAGetLastError() << endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	iResult = send(clientSocket, buf, bufLen, 0);
	if (iResult == SOCKET_ERROR)
	{
		cout << "failed send ACK: " << WSAGetLastError() << endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	// 파일 전송
	FILE* fp;
	errno_t err;
	char fileName[20] = "B.mp4";
	err = fopen_s(&fp, fileName, "wb");		// 파일 열기 오류 검증 변수

	if (err == 0)
	{
		cout << "File open success!: " << fileName << endl;
		do
		{
			Sleep(10);
			iResult = recv(clientSocket, buf, bufLen, 0);

			if (iResult > 0)
			{
				cout << "file receiving.. \n";
				fwrite(buf, sizeof(char), bufLen, fp);
			}
			else
			{
				cout << "Receiving failed with error: " << WSAGetLastError() << endl;
				closesocket(clientSocket);
				WSACleanup();
				return 1;
			}

		} while (iResult != 0);

	}
	else
	{
		cout << "Opening " << fileName << " failed with error!\n";
		return 1;
	}
	

	// 더이상 전송이 없다면 종료
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