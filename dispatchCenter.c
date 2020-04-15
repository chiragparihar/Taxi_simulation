#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>


// Initialize the dispatch center server by creating the server socket, setting up the server address,
// binding the server socket and setting up the server to listen for taxi and customer clients.
void initializeDispatchServer(int *serverSocket, struct sockaddr_in  *serverAddress) {
  int status;
  
  *serverSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if(*serverSocket < 0){
    printf("server error \n");
    exit(-1);
  }
  
  memset(serverAddress, 0, sizeof(*serverAddress)); // zeros the struct
  serverAddress->sin_family = AF_INET;
  serverAddress->sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddress->sin_port = htons((unsigned short) SERVER_PORT);
  status = bind(*serverSocket, (struct sockaddr *)serverAddress,sizeof(*serverAddress));
  if (status < 0) {
    printf("*** SERVER ERROR: Could not bind socket.\n");
    exit(-1);
  }
 
 // Set up the line-up to handle up to 10 clients in line
  status = listen(*serverSocket, 10);
  if (status < 0) {
    printf("*** SERVER ERROR: Could not listen on socket.\n");
    exit(-1);
  }
  
}




// Handle client requests coming in through the server socket.  This code should run
// indefinitiely.  It should wait for a client to send a request, process it, and then
// close the client connection and wait for another client.  The requests that may be
// handled are SHUTDOWN (from stop.c), REQUEST_TAXI (from request.c) and UPDATE or
// REQUEST_CUSTOMER (from taxi.c) as follows:

//   SHUTDOWN - causes the dispatch center to go offline.

//   REQUEST_TAXI - contains 2 additional bytes which are the index of the area to be
//                  picked up in and the index of the area to be dropped off in. If
//                  the maximum number of requests has not been reached, a single YES
//                  byte should be sent back, otherwise NO should be sent back.

//   REQUEST_CUSTOMER - contains no additional bytes.  If there are no pending customer
//                      requests, then NO should be sent back.   Otherwise a YES should
//                      be sent back followed by the pickup and dropoff values for the
//                      request that has been waiting the logest in the queue.

//   UPDATE - contains additional bytes representing the taxi's x, y plate, status and
//            dropoff area.  Nothing needs to be sent back to the taxi.

void *handleIncomingRequests(void *d) {
 
  DispatchCenter  *dispatchCenter = d;

  int                 serverSocket,clientSocket,addrSize;
  struct sockaddr_in  serverAddress,clientAddr;
  char buffer;
  char command;
  char pickup,dropoff;
  int front =0;
  short plate;
  char dropArea,status;
  int x,y;
  // Initialize the server
  initializeDispatchServer(&serverSocket, &serverAddress);

  // REPLACE THE CODE BELOW WITH YOUR OWN CODE
  while (1) {
    addrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket,(struct sockaddr *)&clientAddr,&addrSize);
    if (clientSocket < 0) {
      printf("*** SERVER ERROR: Could accept incoming client connection.\n");
      exit(-1);
    }
    // Go into infinite loop to talk to client
    while (1) {
    // Get the message from the client

      recv(clientSocket, &buffer, 1, 0);
      
      if(buffer == REQUEST_TAXI){
        if(dispatchCenter->numRequests >= MAX_REQUESTS){
          command = NO;
          printf("request full \n");
          send(clientSocket,&command,1,0);
        }
        else{
          command = YES;
          send(clientSocket,&command,1,0);
          recv(clientSocket,&pickup,1,0);
          recv(clientSocket,&dropoff,1,0);
          
          dispatchCenter->requests[(dispatchCenter->numRequests+front)%MAX_REQUESTS].pickupLocation = pickup - 48;
          dispatchCenter->requests[(dispatchCenter->numRequests+front)%MAX_REQUESTS].dropoffLocation = dropoff - 48;
          dispatchCenter->numRequests++;
        }
        break;
      }
      else if(buffer == UPDATE){
        recv(clientSocket,&plate,sizeof(short),0);
        recv(clientSocket,&status,1,0);
        recv(clientSocket,&x,sizeof(int),0);
        recv(clientSocket,&y,sizeof(int),0);
        recv(clientSocket,&dropArea,1,0);
        for(int i=0;i<dispatchCenter->numTaxis;i++){
          if(dispatchCenter->taxis[i]->plateNumber == plate){
            dispatchCenter->taxis[i]->status = status;
            
            dispatchCenter->taxis[i]->x = x;
            dispatchCenter->taxis[i]->y =y;
            dispatchCenter->taxis[i]->dropoffArea = dropArea;
            break;
          }
        }
        break;
      }
      else if(buffer == REQUEST_CUSTOMER){
        if(dispatchCenter->numRequests <= 0){
          command = NO;
          send(clientSocket,&command,1,0);
          
        }
        else{
          command = YES;
          
          send(clientSocket,&command,1,0);
          char p = dispatchCenter->requests[front].pickupLocation;
          char d = dispatchCenter->requests[front].dropoffLocation;
          send(clientSocket,&p,1,0);
          send(clientSocket,&d,1,0);
          dispatchCenter->numRequests--;
          front = (front+1)%MAX_REQUESTS;
          
        }
        break;
        
        
      }
      else if(buffer == SHUTDOWN){
        break;
      }
      else{

      }
    }

    close(clientSocket); // Close this client's socket
    

    if(buffer == SHUTDOWN){
      break;
    }
    
    // Don't forget to close the sockets!
    
  }
  close(serverSocket);
  printf("SERVER: Shutting down.\n");
}
