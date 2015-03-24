#pragma once

// include 
#include <list>       // standard list
#include <WinSock2.h> // socket library

// IPv4 string size "255.255.255.255\0"
#define IP_STRING_SIZE 16
// Max buffer size
#define MAX_BUFFER_SIZE 1024

class ConnectionManager
{
public:
  // to store client information
  typedef struct {
    SOCKET socket;                  // client socket
	char   ip[IP_STRING_SIZE];      // client IP address 
	short  port;                    // client port address
	short  bInUse;                  // buffer spaced in use
	char   buffer[MAX_BUFFER_SIZE]; // client dedicated buffer
  } tClient;


private:
  SOCKET             srvSocket;     // listen server socket
  std::list<tClient> connections;   // clients info
  fd_set             receive_set;   // reception socket set
  int                maxConnection;	// maximum socket info

  // handle a new client connection
  void HandleNewClient();        
  // handle the reception of a new packet / message
  void HandleMessage(const tClient* client); 
  // handle the disconnection of a client
  void HandleDisconnect(const tClient* client);

public:
  // set the server socket in a specific port
  ConnectionManager(short port);
  ~ConnectionManager();

  // Override  
  virtual void ClientMessage(const tClient* client, const char* message, const int msgSize);
  virtual void ClientDisconnect(const tClient* client);
  virtual void ClientConnect(const tClient* client);

  // Handle & Update connections - note this function will freeze 
  // until an event occurs or the specified timeout
  void Update(int timeout_ms);
};

