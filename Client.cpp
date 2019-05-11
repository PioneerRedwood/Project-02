// Windows socket 프로그래밍 클라이언트 코드 <파일 전송>
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

	char buf[DEFAULT_BUFLEN];
	int bufLen = DEFAULT_BUFLEN;

	// 파일 송신
	FILE* fp;
	errno_t err;
	char fileName[20] = "A.png";
	err = fopen_s(&fp, fileName, "r");

	if (err != 0)
	{
		cout << "File open success!: " << fileName << endl;
		// 파일 크기 계산
		int iTest = fseek(fp, 0, SEEK_END);
		if (iTest != 0)
		{
			cout << "Error! failed to calculate: " << fileName << "'s Size..\n";
			return 1;
		}
		int count = ftell(fp);		// 파일 크기 저장
		fseek(fp, 0L, SEEK_SET);	// 파일 포인터 처음으로 옮김
		while (count >= 0)
		{
			// 파일 전송
			ZeroMemory(buf, bufLen);
			// 파일 내용을 읽어서 버퍼에 담기
			fread(buf, sizeof(char), bufLen, fp);
			// 전송
			iResult = send(ConnectSocket, buf, bufLen, 0);
			if (iResult > 0)
			{
				cout << "file sending.. ";
				// 루프 탈출을 위해 버퍼 크기 만큼 카운트 변수를 감소
				count = count - bufLen;
			}
			else if (iResult == 0)
				cout << "Connection closing.. \n";
			else
			{
				cout << "recv failed with error: " << WSAGetLastError() << endl;
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}
		}
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