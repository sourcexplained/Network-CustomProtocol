#include <stdio.h>
#include <WinSock2.h>
#include <ws2tcpip.h>

#include "Packet.h"

#define SERVER_IPADDR "127.0.0.1"
#define SERVER_PORT    8233

int main(int argc, char* argv[]){
	// Windows dependencies
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData); // initiates the use of the Windows Sockets DLL.

	// Server information socket + addresss 
	SOCKET		cltSocket; 	// socket to communicate with the server
	sockaddr_in 	srvAddr;	// server address

	Packet p;
	
	// buffer definition for incoming data
#define BUFFER_SIZE 128
	char	 buffer[BUFFER_SIZE];
	int     msgSize;

	// create a TCP socket information
	cltSocket = socket(AF_INET, SOCK_STREAM, 0);
	// Define the server address
	srvAddr.sin_family = AF_INET;	// Indicated Internet Protocol (IP)
	inet_pton(AF_INET, SERVER_IPADDR, &(srvAddr.sin_addr.s_addr)); // Server address
	srvAddr.sin_port = htons(SERVER_PORT);				  // port definition 

	// Associate the socket with the address we just defined
	if (connect(cltSocket, (sockaddr*)&srvAddr, sizeof(srvAddr)) != 0){
		printf("Failed to connect to server %s:%d\n", SERVER_IPADDR, SERVER_PORT);
		return -1;
	}
	printf("Connection established!\n");

	printf("\nType your message: ");
	// read from the std input a message and place it on the buffer
	while (fgets(buffer, BUFFER_SIZE, stdin)){
		// remove the \n from the user input and make sure is null terminated
		buffer[strlen(buffer) - 1] = '\0';

		if (_strcmpi(buffer,"quit")==0){ // if the message is quit
			break;
		}

		// send a user text message
		send(cltSocket, buffer, strlen(buffer) + 1, 0);

		// receive incoming string message (until a null terminated) 
		msgSize = 0;
		do{
			msgSize += recv(cltSocket, &(buffer[msgSize]), BUFFER_SIZE - msgSize, 0);
		} while (buffer[msgSize - 1] != '\0');
		printf("Received:%s\n\n", buffer);

		// request the user to type a new message
		printf("Type your message: ");
	}

		// close socket
	closesocket(cltSocket);

	// Windows dependencies
	WSACleanup();
	return 0;
}
