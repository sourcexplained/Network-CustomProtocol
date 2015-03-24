#include <stdio.h>
#include "ConnectionManager.h"

#define SERVER_PORT 8233
#define TIMEOUT_MS  5000 

int main(int argc, char* argv[]){
	// Windows dependencies
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData); // initiates the use of the Windows Sockets DLL by a process.

	ConnectionManager cMng(SERVER_PORT);

	// receive incomming String message (null terminated char sequence) from this client 
	do{
		printf("Handle connections\n");
		cMng.Update(TIMEOUT_MS);
	} while (1);


	// Windows dependencies
	WSACleanup();
	return 0;
}
