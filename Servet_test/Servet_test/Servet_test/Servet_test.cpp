// Servet_test.cpp 
#include "stdafx.h"
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 256
#define DEFAULT_PORT "27015"
#define DEFAULT_NODENAME "localhost"
int __cdecl main(void)
{
	printf("<---SERVER--->\n");
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		system("pause");
		return 1;
	}else{
		printf("Initializing Server Winsock succeed\n");
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(DEFAULT_NODENAME, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		system("pause");
		WSACleanup();
		return 1;
	}else{
		printf("Node %s on port %s resolved\n", DEFAULT_NODENAME, DEFAULT_PORT);
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		system("pause");
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		system("pause");
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		system("pause");
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}else {
		printf("Socket`s listening succeed\n");
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		system("pause");
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}else {
		printf("Accept a client socket\n");
	}

	// No longer need server socket
	closesocket(ListenSocket);

	// Receive until the peer shuts down the connection
	do {
		memset(recvbuf, 0, DEFAULT_BUFLEN);
		iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0) {
			printf("Received: %s\n", recvbuf);
		}
		else if (iResult == 0){
			printf("Connection closing...\n");
			system("pause");
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			system("pause");
			closesocket(ClientSocket);
			//WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// Echo the buffer back to the sender
	/*iSendResult = send(ClientSocket, recvbuf, iResult, 0);
	if (iSendResult == SOCKET_ERROR) {
	printf("send failed with error: %d\n", WSAGetLastError());
	system("pause");
	closesocket(ClientSocket);
	WSACleanup();
	return 1;
	}
	printf("Sent: %s\n", recvbuf);
	system("pause");*/

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		system("pause");
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}

