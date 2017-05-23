// Client_Test.cpp 

#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 256
#define DEFAULT_PORT "27015"
#define DEFAULT_NODENAME "localhost"
int __cdecl main(int argc, char **argv)
{
	printf("<---CLIENT--->\n");
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		system("pause");
		return 1;
	}else {
		printf("Initializing Client Winsock succeed\n");
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(DEFAULT_NODENAME, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		system("pause");
		WSACleanup();
		return 1;
	}else {
		printf("Node %s on port %s resolved\n", DEFAULT_NODENAME, DEFAULT_PORT);
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			system("pause");
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		printf("Socket for connecting to server created\n");
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		system("pause");
		WSACleanup();
		return 1;
	}

	int valueArray[15];
	size_t i = 0;
	for (i; i < 14; i++)
	{
		valueArray[i] = rand()%100;
	}

	i = 0;
	char endStr[100];
	do
	{
		char sendbuf[DEFAULT_BUFLEN];
		memset(sendbuf, 0, DEFAULT_BUFLEN);
		
		//Variable changes rules
		if(i%2 == 1)
		{
			valueArray[i] = rand() % 100;
			printf("Variable changes its value once in two cycles.\nVariable number %i change value on: %i\n", i, valueArray[i]);
		}

		if ((i+1) % 5 == 0)
		{
			valueArray[i] = rand() % 100;
			printf("Variable changes its value once in five cycles.\nVariable number %i change value on: %i\n", i, valueArray[i]);
		}

		_itoa_s(valueArray[i], sendbuf, 10);
		// Send an initial buffer
		iResult = send(ConnectSocket, sendbuf, DEFAULT_BUFLEN, 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			system("pause");
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		printf("Sent variable number %i: %s\n",i ,sendbuf);
		if (i < 14)
			i++;
		else
			i = 0;
		
		printf("If you want to end sending, please write 'end' in this line: ");
		gets_s(endStr);
	} while (!(endStr[0] =='e'&& endStr[1] == 'n' &&endStr[2] == 'd'));

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		system("pause");
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive until the peer closes the connection
	do {
		memset(recvbuf, 0, DEFAULT_BUFLEN);
		iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0){
			printf("Bytes received: %s\n", recvbuf);
			system("pause");
		}
		else if (iResult == 0) {
			printf("Connection closed\n"); 
			system("pause");
		}
		else{
			printf("recv failed with error: %d\n", WSAGetLastError());
			system("pause");
		}

	} while (iResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}
