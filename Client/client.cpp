#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h> // �������� ����������� �������, �������� � ����������� Winsock
#include <ws2tcpip.h> // �������� �����������, �������������� � ��������� ���������� WinSock Protocol-Specific 2 ��� TCP/IP, ������� �������� ����� ����� ������� � ���������, ������������ ��� ���������� IP-�������.
#include <stdio.h> // ������������ ��� ����������� ������� � �������� ������, � ��������� ������� printf().
#include <stdlib.h>
#pragma comment(lib, "Ws2_32.lib") // ����������� #pragma ��������� ������������, ��� ��������� ���� Ws2_32.lib.
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512


int __cdecl main(int argc, char** argv) {

	// ������� ������ WSADATA � ������ wsaData.
	WSADATA wsaData;
	// ������ ������ SOCKET � ������ ConnectSocket
	SOCKET ConnectSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;
	int recvbuflen = DEFAULT_BUFLEN;
	const char* sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;

	//�������� WSAStartup � ���������� ��� �������� � ���� ������ �����, ����� ��������� ������� ������.
	

	// ������������� Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// ���������� ����� � ���� �������
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	

	// ������� ����������� � ������ �� ��� ���, ���� �� ����� ���������� �����.
	// �������� ������� ������ � ���������� � �������� � ���������� ConnectSocket
	// ������� ������������ � ������� ������, �������������
	// ������� getaddrinfo

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// �������� ������ ��� ����������� � �������
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// �������� ������� connect , ������� � �������� ���������� ��������� ����� � ��������� sockaddr . ��������� ������� ����� ������.
	// ������������ � �������
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup(); // WSACleanup ������������ ��� ����������� ������������� ���������� DLL WS2_32.
		return 1;
	}

	// �������� � ��������� ������ �� �������
	// ������� send � recv , ������������ �������� ����� ������������ ����������
	// ��������� ��������� �����
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	// ��������� ���������� ��� ��������, ��� ��� ������ ������ ������������ �� �����
	// ������ ��� ����� ����� ������������ ConnectSocket ��� ��������� ������

	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// �������� ������, ���� ������ �� ������� ����������
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0) {
			printf("Connection closed\n");
		}
		else {
			printf("recv failed: %d\n", WSAGetLastError());
		}
	} while (iResult > 0);

	// ����� ���������� �������� � ��������� ������ ������ ����������� �� ������� � ��������� ������ ������.
		closesocket(ConnectSocket);
		WSACleanup();
	return 0;
}