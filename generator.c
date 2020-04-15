#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "simulator.h"

void main() {
  // Set up the random seed
  srand(time(NULL));

  while(1) {
    char x[2];
    char y[2];
    x[0] = 48 + (rand() % 6);
    x[1] = 0;
    y[0] = 48 + (rand() % 6);
    y[1] = 0;
    // ...
    // add code here
    // ...
    printf("x is %s \n", x);
    printf("y is %s \n", y);
    char command[100];
    strcpy(command,"./customer ");

    strcat(command,x);
    strcat(command," ");
    strcat(command,y);
    printf("%s \n",command);
    
    system(command);
    usleep(250000);   /// do not remove this
  }
}
