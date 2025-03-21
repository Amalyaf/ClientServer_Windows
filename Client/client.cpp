#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h> // содержит большинство функций, структур и определений Winsock
#include <ws2tcpip.h> // содержит определения, представленные в документе Приложения WinSock Protocol-Specific 2 для TCP/IP, который включает более новые функции и структуры, используемые для извлечения IP-адресов.
#include <stdio.h> // используется для стандартных входных и выходных данных, в частности функции printf().
#include <stdlib.h>
#pragma comment(lib, "Ws2_32.lib") // Комментарий #pragma указывает компоновщику, что требуется файл Ws2_32.lib.
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512


int __cdecl main(int argc, char** argv) {

	// Создали объект WSADATA с именем wsaData.
	WSADATA wsaData;
	// Создаём объект SOCKET с именем ConnectSocket
	SOCKET ConnectSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;
	int recvbuflen = DEFAULT_BUFLEN;
	const char* sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;

	//Вызываем WSAStartup и возвращаем его значение в виде целого числа, чтобы проверить наличие ошибок.
	

	// Инициализация Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Определяем адрес и порт сервера
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	

	// Попытка подключения к адресу до тех пор, пока не будет достигнута успех.
	// Вызываем функцию сокета и возвращаем её значение в переменную ConnectSocket
	// Попытка подключиться к первому адресу, возвращенному
	// вызовом getaddrinfo

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Создание СОКЕТА для подключения к серверу
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Вызываем функцию connect , передав в качестве параметров созданный сокет и структуру sockaddr . Проверяем наличие общих ошибок.
	// Подключаемся к серверу
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
		WSACleanup(); // WSACleanup используется для прекращения использования библиотеки DLL WS2_32.
		return 1;
	}

	// Отправка и получение данных на клиенте
	// Функции send и recv , используемые клиентом после установления соединения
	// Отправить начальный буфер
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	// отключаем соединение для отправки, так как больше данные отправляться не будут
	// клиент все равно может использовать ConnectSocket для получения данных

	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Получаем данные, пока сервер не закроет соединение
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

	// После завершения отправки и получения данных клиент отключается от сервера и завершает работу сокета.
		closesocket(ConnectSocket);
		WSACleanup();
	return 0;
}