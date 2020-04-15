#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "simulator.h"


void main() {
  // Contact the dispatch center and ask it to shut down
  // Create socket
  int clientSocket;
  struct sockaddr_in serverAddress;
  char command = SHUTDOWN;
  int status;
  clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientSocket < 0) {
    printf("*** CLIENT ERROR: Could not open socket.\n");
    exit(-1);
  }
  // Setup address
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
  serverAddress.sin_port = htons((unsigned short) SERVER_PORT);
 // Connect to server
  status = connect(clientSocket, (struct sockaddr *) &serverAddress,sizeof(serverAddress));
  if (status < 0) {
    printf("*** CLIENT ERROR: Could not connect.\n");
    exit(-1);
  }
  // Go into loop to commuincate with server now
 
  send(clientSocket,&command,1,0);
  // Get response from server, should be "OK"
  close(clientSocket);
  
}

