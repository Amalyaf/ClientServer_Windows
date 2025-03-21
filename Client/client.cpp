#include <winsock2.h> // �������� ����������� �������, �������� � ����������� Winsock
#include <ws2tcpip.h> // �������� �����������, �������������� � ��������� ���������� WinSock Protocol-Specific 2 ��� TCP/IP, ������� �������� ����� ����� ������� � ���������, ������������ ��� ���������� IP-�������.
#include <stdio.h> // ������������ ��� ����������� ������� � �������� ������, � ��������� ������� printf().

#pragma comment(lib, "Ws2_32.lib") // ����������� #pragma ��������� ������������, ��� ��������� ���� Ws2_32.lib.
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512


int __cdecl main(int argc, char** argv) {

	// ������� ������ WSADATA � ������ wsaData.
	WSADATA wsaData;

	//�������� WSAStartup � ���������� ��� �������� � ���� ������ �����, ����� ��������� ������� ������.
	int iResult;

	// ������������� Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;
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

	// ������ ������ SOCKET � ������ ConnectSocket
	SOCKET ConnectSocket = INVALID_SOCKET;

	// �������� ������� ������ � ���������� � �������� � ���������� ConnectSocket
	// ������� ������������ � ������� ������, �������������
    // ������� getaddrinfo
	ptr = result;

	// �������� ������ ��� ����������� � �������
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	// �������� �� ������� ������, ����� ���������, ��� ����� �������� ��������.
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError()); // WSAGetLastError ���������� ����� ������, ��������� � ��������� �������
		freeaddrinfo(result);
		WSACleanup(); // WSACleanup ������������ ��� ����������� ������������� ���������� DLL WS2_32.
		return 1;
	}

	// �������� ������� connect , ������� � �������� ���������� ��������� ����� � ��������� sockaddr . ��������� ������� ����� ������.
	// ������������ � �������
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// �������� � ��������� ������ �� �������
	// ������� send � recv , ������������ �������� ����� ������������ ����������

	int recvbuflen = DEFAULT_BUFLEN;
	const char* sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];

	// ��������� ��������� �����
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	// ��������� ���������� ��� ��������, ��� ��� ������ ������ ������������ �� �����
    // ������ ��� ����� ����� ������������ ConnectSocket ��� ��������� ������

	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
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
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// �������� �������  WSACleanup ��� ������������ �������� 
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}