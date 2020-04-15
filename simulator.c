#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "simulator.h"

#include "taxi.c"
#include "display.c"
#include "dispatchCenter.c"

int main() {
  DispatchCenter     ottawaDispatch;
  int parent;
  pthread_t req;
  pthread_t dis;
  srand(time(NULL));
  parent = getpid();
  ottawaDispatch.numTaxis = 0;
  ottawaDispatch.numRequests = 0;
  ottawaDispatch.numTaxis = 10;
  pthread_create(&dis,NULL,showSimulation,&ottawaDispatch);
  pthread_create(&req,NULL,handleIncomingRequests,&ottawaDispatch);
 
  for(int i=0;i<10;i++){
    Taxi *t = malloc(sizeof(Taxi));
    if(getpid() == parent){
      
      char random = rand() % NUM_CITY_AREAS;
      t->currentArea = random;
      t->pickupArea = UNKNOWN_AREA;
      t->dropoffArea = UNKNOWN_AREA;
      t->plateNumber = i;
      t->status = AVAILABLE;
      t->x = AREA_X_LOCATIONS[random];
      t->y = AREA_Y_LOCATIONS[random];
      ottawaDispatch.taxis[i] = t;
      ottawaDispatch.taxis[i]->pID = fork();
      
    }
    if(ottawaDispatch.taxis[i]->pID == 0){
      runTaxi(ottawaDispatch.taxis[i]);
      free(t);
    }
  }
  

  
  if(getpid() == parent){
    pthread_join(req,NULL);
   
    for(int i=0;i<10;i++){
      kill(ottawaDispatch.taxis[i]->pID,SIGKILL);
    }
    printf("Simulation complete.\n");
  }
  //free(t);
   // kill(t1->pID,SIGKILL);
  // Kill all the taxi processes
  // ...

  // Free the memory
  // ...
    
  //}
}
