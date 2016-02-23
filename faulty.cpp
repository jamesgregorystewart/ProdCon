#include <string.h>
#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <mutex>

using namespace std;

//Functions
void setProgramSpecs();
void *fillBuffer(void *ptr);


//Global Variables
int** buffer;
int bufferLength;
const char** gargv;
int gargc;
int itemsPerProducer;
int numberOfProducers;
int numberOfFaulties;
int numberOfConsumers;
bool showDebug = false;
pthread_mutex_t random_mutex;


int main(int argc, char const *argv[]) {
  gargc = argc;
  gargv = argv;

  //get the flags and values from execution input
  setProgramSpecs();

  //initialize buffer here

  cout << "Starting Threads..." << endl;
  printf("\n");

  //create producer threads
  for (int jj = 0; jj < numberOfProducers; jj++) {
    //pthread_create(&someName, NULL, *fillBuffer, NULL);
  }

  //Create faulty producer threads
  for (int jj = 0; jj < numberOfFaulties; jj++) {
    //create threads in loop
  }

  //Join threads
  //

  return 0;
}

/*******************************************************************


*******************************************************************/

//initializes the buffer and simulation stats
void setProgramSpecs() {
  int ii;
  for (ii = 1; ii < gargc; ii++) {
    if (strcmp(gargv[ii], "-n") == 0) {
      itemsPerProducer = atoi(gargv[ii+1]);
      ii++;
    } else if (strcmp(gargv[ii], "-l") == 0) {
      bufferLength = atoi(gargv[ii+1]);
      ii++;
    } else if (strcmp(gargv[ii], "-p") == 0) {
      numberOfProducers = atoi(gargv[ii+1]);
      ii++;
    } else if (strcmp(gargv[ii], "-f") == 0) {
      numberOfFaulties = atoi(gargv[ii+1]);
      ii++;
    } else if (strcmp(gargv[ii], "-c") == 0) {
      numberOfConsumers = atoi(gargv[ii+1]);
      ii++;
    } else if (strcmp(gargv[ii],"-d") == 0) {
      showDebug = true;
    } else {
      ii++;
    }
  }
}


void *fillBuffer(void *ptr) {
  int randomNumber;

  //use mutex to make sure only one producer is working at a time.
  while(true) {
    pthread_mutex_lock( &random_mutex );
    randomNumber = rand() % 999999 + 2;
    pthread_mutex_unlock( &random_mutex );
  }

}
