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

// This represents a customer request.  There should be two command line arguments representing the
// pickup location and dropoff location.  Each should be a number from 0 to the NUM_CITY_AREAS.
void main(int argc, char *argv[]) {
  if(argc < 3 || argc > 4){
    printf("please rerun with only 2 command line arguments \n");
    exit(-1);
  }
  if((int) argv[1][0] > 53 || (int) argv[1][0] < 48 || (int) argv[2][0] > 53 || (int) argv[2][0] < 48 || argv[1][1] != 0 || argv[2][1] != 0){
    printf("please rerun with valid command line arguments only \n");
    exit(-1);
  }


  int                 clientSocket;  // client socket id
  struct sockaddr_in  clientAddress; // client address
  char command = REQUEST_TAXI;
  char rec;
  int status;
  char pickup = argv[1][0];
  char dropoff = argv[2][0];
  clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientSocket < 0) {
    printf("*** CLIENT ERROR: Could not open socket.\n");
    exit(-1);
  }
  // Setup address
  memset(&clientAddress, 0, sizeof(clientAddress));
  clientAddress.sin_family = AF_INET;
  clientAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
  clientAddress.sin_port = htons((unsigned short) SERVER_PORT);
 // Connect to server
  status = connect(clientSocket, (struct sockaddr *) &clientAddress,sizeof(clientAddress));
  if (status < 0) {
    printf("*** CLIENT ERROR: Could not connect.\n");
    exit(-1);
  }
  // Go into loop to commuincate with server now
 
  send(clientSocket,&command,1,0);
  // Get response from server, should be "OK"
  recv(clientSocket,&rec,1,0);
  if(rec == NO){
    printf("Cannot request taxi\n");
  }
  else{
    send(clientSocket,&pickup,1,0);
    send(clientSocket,&dropoff,1,0);
  }
  close(clientSocket);
  
}



