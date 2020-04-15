#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// The dispatch center server connection ... when made
int                 clientSocket;  // client socket id
struct sockaddr_in  clientAddress; // client address



// Set up a client socket and connect to the dispatch center server.  Return -1 if there was an error.
int connectToDispatchCenter(int *clientSocket,  struct sockaddr_in *serverAddress) {
  int status;
  *clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (*clientSocket < 0) {
    printf("*** CLIENT ERROR: Could not open socket.\n");
    return -1;
  }
  // Setup address
  memset(serverAddress, 0, sizeof(*serverAddress));
  serverAddress->sin_family = AF_INET;
  serverAddress->sin_addr.s_addr = inet_addr(SERVER_IP);
  serverAddress->sin_port = htons((unsigned short) SERVER_PORT);
 // Connect to server
  status = connect(*clientSocket, (struct sockaddr *) serverAddress,sizeof(*serverAddress));
  if (status < 0) {
    printf("*** CLIENT ERROR: Could not connect.\n");
    return -1;
  }
  return 0;

}


void moveTaxi(Taxi *t){
  int deltaX,deltaY;
  if(t->status == PICKING_UP){
    deltaX = (AREA_X_LOCATIONS[t->pickupArea] - AREA_X_LOCATIONS[t->currentArea])/TIME_ESTIMATES[t->currentArea][t->pickupArea];
    deltaY = (AREA_Y_LOCATIONS[t->pickupArea] - AREA_Y_LOCATIONS[t->currentArea])/TIME_ESTIMATES[t->currentArea][t->pickupArea];
  }
  else{
    deltaX = (AREA_X_LOCATIONS[t->dropoffArea] - AREA_X_LOCATIONS[t->pickupArea])/TIME_ESTIMATES[t->pickupArea][t->dropoffArea];
    deltaY = (AREA_Y_LOCATIONS[t->dropoffArea] - AREA_Y_LOCATIONS[t->pickupArea])/TIME_ESTIMATES[t->pickupArea][t->dropoffArea];
  }
  t->x = t->x + deltaX;
  t->y = t->y + deltaY;
  //printf("taxis x and y are %d and %d\n",t->x,t->y);
  t->eta = t->eta - 1;
  if(t->eta <= 0){
    if(t->status == DROPPING_OFF){
      t->x = AREA_X_LOCATIONS[t->dropoffArea];
      t->y = AREA_Y_LOCATIONS[t->dropoffArea];
      t->status = AVAILABLE;
      t->currentArea = t->dropoffArea;
      t->pickupArea = UNKNOWN_AREA;
      t->dropoffArea = UNKNOWN_AREA;
    }
    else if(t->status == PICKING_UP){
      t->x = AREA_X_LOCATIONS[t->pickupArea];
      t->y = AREA_Y_LOCATIONS[t->pickupArea];
      t->status = DROPPING_OFF;
      t->currentArea = UNKNOWN_AREA;
      t->eta = TIME_ESTIMATES[t->pickupArea][t->dropoffArea] ;
    }
    else{

    }
  }
}



// This code runs the taxi forever ... until the process is killed
void runTaxi(Taxi *taxi) {
  // Copy the data over from this Taxi to a local copy
  Taxi   thisTaxi;
  thisTaxi.plateNumber = taxi->plateNumber;
  thisTaxi.currentArea = taxi->currentArea;
  thisTaxi.x = taxi->x;
  thisTaxi.y = taxi->y;
  thisTaxi.status = AVAILABLE;
  thisTaxi.pickupArea = UNKNOWN_AREA;
  thisTaxi.dropoffArea = UNKNOWN_AREA;
  thisTaxi.eta = 0;
  int clientSocket;
  struct sockaddr_in serverAddress;
  char command,rec,pickup,dropoff;
  // Go into an infinite loop to request customers from dispatch center when this taxi is available
  // as well as sending location updates to the dispatch center when this taxi is picking up or dropping off.
  while(1) {
    
    // WRITE SOME CODE HERE
    usleep(50000);  // A delay to slow things down a little
    int status = connectToDispatchCenter(&clientSocket,&serverAddress);
    if(status < 0 ){
      exit(0);
    }
    if(thisTaxi.status == AVAILABLE){
      command = REQUEST_CUSTOMER;
      send(clientSocket,&command,1,0);
      recv(clientSocket,&rec,1,0);
      if(rec == YES){
        recv(clientSocket,&pickup,1,0);
        recv(clientSocket,&dropoff,1,0);
        
        thisTaxi.pickupArea = pickup;
        thisTaxi.dropoffArea = dropoff;
        if(thisTaxi.currentArea == pickup){
          thisTaxi.eta = TIME_ESTIMATES[pickup][dropoff];
         
          thisTaxi.status = DROPPING_OFF;
        }
        else{
          
          thisTaxi.status = PICKING_UP;
          thisTaxi.eta = TIME_ESTIMATES[thisTaxi.currentArea][pickup];
        }
      }
      else{

      }
    }
    else if(thisTaxi.status == DROPPING_OFF || thisTaxi.status == PICKING_UP){
      moveTaxi(&thisTaxi);
      command = UPDATE;
      send(clientSocket,&command,1,0);
      send(clientSocket,&(thisTaxi.plateNumber),sizeof(short),0);
      send(clientSocket,&(thisTaxi.status),1,0);
      
      send(clientSocket,&(thisTaxi.x),sizeof(int),0);
      send(clientSocket,&(thisTaxi.y),sizeof(int),0);
      send(clientSocket,&(thisTaxi.dropoffArea),1,0);
      //moveTaxi(&thisTaxi);

    }
    else{

    }
    //moveTaxi(&thisTaxi);

  }
}


