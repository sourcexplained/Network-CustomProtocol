#include "ConnectionManager.h"

#include <ws2tcpip.h>

ConnectionManager::ConnectionManager(short port){
	// The constructor will start the listener server socket 

	// Server information socket + addresss 
	sockaddr_in srvAddr;	// server address

	// create a TCP socket information
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);
	// Define the local address
	srvAddr.sin_family = AF_INET;		   // Indicated Internet Protocol (IP)
	srvAddr.sin_addr.s_addr = INADDR_ANY;  // Server side address wildcard
	srvAddr.sin_port = htons(port);        // port definition 

	// Associate the socket with the address we just defined
	bind(srvSocket, (sockaddr*)&srvAddr, sizeof(srvAddr));

	// active the socket (open the port), this will allow clients to connect to it
	listen(srvSocket, 8); // 8 defines the max size of waiting queue for incoming calls

	// the maxConnection is just the >= than the larger socket id
	// that it is in use by this connection manager
	maxConnection = srvSocket;

	// recap class attributes
	// srvSocket was initialize abobe as the server listenner 
	// connections is empty 
	// maxConnection = srvSocket (>= than the larger socket id)
}

ConnectionManager::~ConnectionManager(){
	// make sure all connections are correctly destroyed

	// destroy server listener socket
	closesocket(srvSocket);

	// clear all clients
	std::list<tClient>::const_iterator sfd, end;
	for (sfd = connections.begin(), end = connections.end(); sfd != end; ++sfd) {
		closesocket((*sfd).socket);
	}
	// clear the list
	connections.clear();
}

void ConnectionManager::HandleNewClient(){
	sockaddr_in cltAddr;	// client address
	tClient     newClient;
	int			cltAddrSize = sizeof(cltAddr); // size of cltAddr

	// Accept a new incomming connection 
	// ATTN: accept is a preemptive function, it will block this thread until a new connection is received
	newClient.socket = accept(srvSocket, (sockaddr*)&cltAddr, &cltAddrSize);

	// now get it back and print it
	inet_ntop(AF_INET, &(cltAddr.sin_addr), newClient.ip, INET_ADDRSTRLEN);
	newClient.port = ntohs(cltAddr.sin_port);
	newClient.buffer[0] = '\0'; // set the first char in the buffer to '\0'
	newClient.bInUse = 0;       // set the number of buffer positions in use to 0

	// Add to the list of client connections
	connections.push_back(newClient);
	// update max connection (note max connection always has to be >= than max socket id)
	if (maxConnection < (int)newClient.socket){ 
		maxConnection = (int)newClient.socket;
	}

	// call virtual function 
	ClientConnect(&newClient);
}

void ConnectionManager::HandleDisconnect(const tClient* client){
	// call virtual function  
	ClientDisconnect(client);

	// close socket
	closesocket(client->socket);
}

void ConnectionManager::HandleMessage(const tClient* client){
	// receive the message in the client socket
	int msgSize = recv(client->socket, (char*)&(client->buffer[client->bInUse]), MAX_BUFFER_SIZE - client->bInUse, 0);
	((tClient*)client)->bInUse += msgSize;

	// check if message arrived in full, if so call Client Message
	if (client->buffer[client->bInUse - 1] == '\0'){
		ClientMessage(client, client->buffer, client->bInUse);
	}

	// the message was handled, reset the buffer to be able to read future messages
	((tClient*)client)->buffer[0] = '\0';
	((tClient*)client)->bInUse = 0;
}


void ConnectionManager::ClientConnect(const tClient* client){
	printf("Client Connect %i = %s:%d!\n", client->socket, client->ip, client->port);
}

void ConnectionManager::ClientDisconnect(const tClient* client){
	printf("Client disconnect %i = %s:%d!\n", client->socket, client->ip, client->port);
}

void ConnectionManager::ClientMessage(const tClient* client, const char* message, const int msgSize) {
	char localBuffer[MAX_BUFFER_SIZE];

	printf("New message from %i = %s:%d! Size:%i\n", client->socket, client->ip, client->port, msgSize);
	printf("Message:|%s|\n", message);

	// set a new message in a local buffer
	sprintf_s(localBuffer, MAX_BUFFER_SIZE,"Echo of message: %s", message);
	// send message echo
	send(client->socket, localBuffer, strlen(localBuffer) + 1, 0);
}

void ConnectionManager::Update(int timeout_ms){
	std::list<tClient>::const_iterator sfd, end;
	fd_set  receive_set;
	u_long  bufferSize;
	int     status;
	timeval timeout;

	// reset reception set
	FD_ZERO(&receive_set);
	// add the server socket to the list
	FD_SET(srvSocket, &receive_set);
	// add the connections to the connection set
	for (sfd = connections.begin(), end = connections.end(); sfd != end; ++sfd) {
		FD_SET((*sfd).socket, &receive_set);
	}

	// set the timeout 
	timeout.tv_sec = timeout_ms / 1000;
	timeout.tv_usec = (timeout_ms % 1000) * 1000;

	// wait until timout or reception of message
	select(maxConnection + 1, &receive_set, NULL, NULL, &timeout);

	// check if new incoming connection
	if (FD_ISSET(srvSocket, &receive_set)){
		HandleNewClient();
	}

	// get connection list
	sfd = connections.begin();
	end = connections.end();
	// add the connections to the connection set
	while (sfd != end){	// loop through all client connections

		// if flag is set means there is activity in that channel
		if (FD_ISSET((*sfd).socket, &receive_set)){
			// check channel status and message size 
			status = ioctlsocket((*sfd).socket, FIONREAD, &bufferSize);
			if (status != 0 || bufferSize <= 0){ // invalid drop the connection
				HandleDisconnect(&(*sfd));
				// erase the client connection entry
				connections.erase(sfd++);
				continue;
			} 
			// if all if fine, handle the message receive
			HandleMessage(&(*sfd));
		}
		// next connection
		++sfd;
	}
}

