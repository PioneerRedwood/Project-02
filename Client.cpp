// Windows socket 프로그래밍 클라이언트 코드 <파일 전송>
#define WIN32_LEAN_AND_MEAN

#include<iostream>
#include<string>
#include<ws2tcpip.h>
#include<winsock2.h>
#include<filesystem>
#include<Windows.h>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 4096
#define DEFAULT_PORT "9000"

int main()
{
	WSADATA wsaData;
	int iResult;
	char buf[DEFAULT_BUFLEN];
	int bufLen = DEFAULT_BUFLEN;

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

	// 서버 주소와 포트를 설정
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

	// 서버에 연결
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Unable to connect to server!" << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	cout << "We'll start to sending a file.\n";
	
	FILE* fp;
	errno_t err;
	char fileName[20] = "A.mp4";
	err = fopen_s(&fp, fileName, "rb");		// 파일 열기 오류 검증 변수

	if (err == 0)
	{
		cout << "File open success!: " << fileName << endl;
		// 파일 크기 계산
		int iTest = fseek(fp, 0, SEEK_END);
		if (iTest != 0)
		{
			cout << "Error! failed to calculate: " << fileName << "'s Size..\n";
			return 1;
		}
		double fileSize = ftell(fp);		
		cout << fileName << " size: " << fileSize << endl;
		fseek(fp, 0, SEEK_SET);
		int present;
		double total = fileSize;

		ZeroMemory(buf, bufLen);
		do
		{
			// 요청 전송
			iResult = send(ConnectSocket, buf, bufLen, 0);
			if (iResult == SOCKET_ERROR)
			{
				cout << "failed send SYN: " << WSAGetLastError() << endl;
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}

			// 응답 받기
			iResult = recv(ConnectSocket, buf, bufLen, 0);
			if (iResult == SOCKET_ERROR)
			{
				cout << "failed recv SYN: " << WSAGetLastError() << endl;
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}
			
			fread(buf, sizeof(char), bufLen, fp);
			iResult = send(ConnectSocket, buf, bufLen, 0);
			
			// 전송
			if (iResult > 0)
			{
				cout << "file sending.. \n";
				fileSize -= iResult;
				present = ftell(fp);
				cout << "Sent: " << iResult	<< "  present: " << (present / total) * 100 << "% " << endl;
			}
			else
			{
				cout << "Sending failed with error: " << WSAGetLastError() << endl;
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}
 		} while (fileSize > 0);
	}
	else
	{
		cout << "Opening " << fileName << " failed with error!\n";
		return 1;
	}
	

	// 더이상 전송할 데이터가 없을 때 소켓 닫기
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "shutdown failed: " << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// 종료
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}