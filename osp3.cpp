#include <string.h>
#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <mutex>
#include <math.h>
#include <queue>
#include <string.h>
#include <sstream>

using namespace std;

//Functions
void setProgramSpecs();

bool isIntegerPrime(int toTest);
bool isIntegerEven(int toTest);
int getPrimeNumber();

void *producerFunction(void *ptr);
void *faultyProducerFunction(void *ptr);
void *consumerFunction(void *ptr);
void *fillBuffer(void *ptr);

pthread_mutex_t buffer_mutex;
pthread_mutex_t print_mutex;

//*******************************************************************
//
//                      Custom Buffer class
//
//    Wrapped around the STL Queue class to provide a max size for the
//    queue.
//
//*******************************************************************/
class CustomBuffer: public std::queue<int> {
  public:
    // Public functions
    CustomBuffer(){
      // Constructor...initialize variables
      size = 0;
      currentNumberOfEntries = 0;
    }

    void setMaxSize(int newSize){
      // Set the max size of the queue
      printf("Setting max size to %i...\n", newSize);
       size = newSize;
    }

    bool bufferIsFull(){
      //printf("Checking if the buffer is full. Entries: %i Size: %i.\n", currentNumberOfEntries, size);
      if(currentNumberOfEntries >= size){
        return true;
      } else {
        return false;
      }
    }

    bool bufferIsEmpty(){
      if(currentNumberOfEntries == 0){
        return true;
      } else {
        return false;
      }
    }

    int getNumberOfItemsInBuffer(){
      return currentNumberOfEntries;
    }

    string printBuffer(){

      string temp = "";

      //temp = sprintf("(%i): [ ", currentNumberOfEntries);

      ostringstream os;
      os << "(" << currentNumberOfEntries << "): [ ";

      // Obtain mutex so queue doesn't change while we print it out
      //pthread_mutex_lock( &buffer_mutex );

      pthread_mutex_lock( &buffer_mutex );

      for(int ii = 0; ii < currentNumberOfEntries; ii++){
        int temp = buffer.front();
        buffer.pop();

        os << temp << " ";
        printf("%i\n", temp);

        buffer.push(temp);
      }

      pthread_mutex_unlock( &buffer_mutex );

      // Release mutex
      //pthread_mutex_unlock( &buffer_mutex );

      os << "]\n";

      string s = os.str();

      return s;
    }

    void addItem(int item){
      // If the buffer is not full, then use mutex locks to add to buffer
      if(!bufferIsFull()){
        // Obtain lock
        pthread_mutex_lock( &buffer_mutex );

        printf("Adding %i\n", item);

        // Critical section
        buffer.push(item);
        currentNumberOfEntries++;

        // Release lock
        pthread_mutex_unlock( &buffer_mutex );
      } else {
        cout << "Error adding item: buffer full!\n";
      }
    }

    int removeItem(){
      if(!bufferIsEmpty()){

        pthread_mutex_lock( &buffer_mutex );
        int poppedItem = buffer.front();
        buffer.pop();
        pthread_mutex_unlock( &buffer_mutex );

        currentNumberOfEntries--;

        return poppedItem;
      } else {
        cout << "Error removing item: buffer empty!\n";
        return -1;
      }
    }

  protected:
    int size;
    int currentNumberOfEntries;
    std::queue<int> buffer;
};

// ************************** Global Variables **************************
CustomBuffer buffer;
int bufferLength;
const char** gargv;
int gargc;

int itemsPerProducer;
int numberOfProducers;
int numberOfFaulties;
int numberOfConsumers;
int totalNumberOfItems;

bool showDebug = false;


int main(int argc, char const *argv[]) {
  gargc = argc;
  gargv = argv;

  //get the flags and values from execution input
  setProgramSpecs();
  buffer.setMaxSize(bufferLength);
  pthread_t producers[numberOfProducers];
  pthread_t faultiers[numberOfFaulties];
  pthread_t consumers[numberOfConsumers];

  totalNumberOfItems = itemsPerProducer * (numberOfProducers + numberOfFaulties);

  // Initialize mutex
  pthread_mutex_init(&buffer_mutex, NULL);
  pthread_mutex_init(&print_mutex, NULL);

  cout << "Starting Threads..." << endl;
  printf("\n");

  // Create producer threads
  int jj;
  for (int jj = 0; jj < numberOfProducers; jj++) {
    pthread_create(&producers[jj], NULL, *producerFunction, new int(jj));
  }

  // Create faulty producer threads
  // for (int jj = numberOfProducers; jj < numberOfFaulties + numberOfProducers; jj++) {
  //   pthread_create(&faultiers[jj], NULL, *faultyProducerFunction, new int(jj));
  // }

  // Create consumers
  for(int jj = numberOfProducers + numberOfFaulties; jj < numberOfConsumers + numberOfFaulties + numberOfProducers; jj++) {
    pthread_create(&consumers[jj], NULL, *consumerFunction, new int(jj));
  }

  // Join with producer threads
  for(jj = 0; jj < numberOfProducers; jj++){
    pthread_join(producers[jj], NULL);
  }

  // Join with faulty producers
  for(jj = 0; jj < numberOfFaulties; jj++){
    pthread_join(faultiers[jj], NULL);
  }

  // Join with consumers
  for(jj = 0; jj < numberOfFaulties; jj++){
    pthread_join(consumers[jj], NULL);
  }

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
      printf("Buffer length: %i\n", bufferLength);
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

//*******************************************************************
//
//                      Producer Functions
//
//*******************************************************************/

void *producerFunction(void *ptr){

  // Get ID
  int id = *(int*)ptr;      //cast the int object into an id
  delete (int*)ptr;         //delete the int object like I read online

  // Create and add the correct number of items to
  // the buffer.
  for(int ii = 0; ii < itemsPerProducer; ii++){
    ostringstream os;

    // Generate prime number
    int primeNumber = getPrimeNumber();

    // Add to buffer when possible
    bool done = false;

    while(!done){
      // Busy wait for when buffer is not full

      // If the buffer is not full...
      if(!buffer.bufferIsFull()){
        // Add the prime number to the buffer
        // (buffer handles the mutexes for us)
        buffer.addItem(primeNumber);

        // Now print that we have added it
        os << "PRODUCER " << id << " writes " << (ii + 1) << "/" << itemsPerProducer << "   " << primeNumber;
        string output = os.str();
        string bufferOutput = buffer.printBuffer();

        output = output + bufferOutput;

        pthread_mutex_lock( &print_mutex );
        printf("%s", output.c_str());
        pthread_mutex_unlock( &print_mutex );

        done = true;
      } else {
        done = false;
        //pthread_mutex_lock( &print_mutex );
        //printf("PRODUCER %i: Buffer is full, busy waiting till there is an empty slot\n", id);
        //pthread_mutex_unlock( &print_mutex );
      }
    }
  }
  // We have now done our work. Exit thread.
  pthread_exit(0);
}

void *faultyProducerFunction(void *ptr){

  // Get ID
  int id = *(int*)ptr;      //cast the int object into an id
  delete (int*)ptr;         //delete the int object like I read online

  id = id - numberOfProducers;

  // Create and add the correct number of items to
  // the buffer.
  for(int ii = 0; ii < itemsPerProducer; ii++){

    int number;

    // Generate number
    bool doneMakingNumber = false;
    while(!doneMakingNumber){
      number = rand() % 999999;

      if(isIntegerEven(number)){
        doneMakingNumber = true;
      } else {
        doneMakingNumber = false;
      }
    }

    // Add to buffer when possible
    bool done = false;
    while(!done){
      // Busy wait for when buffer is not full

      // If the buffer is not full...
      if(!buffer.bufferIsFull()){
        // Add the number to the buffer
        // (buffer handles the mutexes for us)
        buffer.addItem(number);

        cout << "Add from PR*D*C*R\n";

        // Now print that we have added it
        pthread_mutex_lock( &print_mutex );
        printf("PR*D*C*R %i writes %i/%i   %i: ", id, (ii + 1) , itemsPerProducer, number);
        buffer.printBuffer();
        pthread_mutex_unlock( &print_mutex );

        done = true;
      } else {
        done = false;
      }
    }
  }
  // We have now done our work. Exit thread.
  pthread_exit(0);

}

int getPrimeNumber(){
  // Returns a prime number
  bool done = false;
  int result = -1;

  while(!done){
    // Generate a random positive integer
    result = rand() % 999999;

    // If this int is prime, we are done
    done = isIntegerPrime(result);
  }

  return result;
}

//*******************************************************************
//
//                      Consumer Functions
//
//*******************************************************************/

void *consumerFunction(void *ptr){

  // Get ID
  int id = *(int*)ptr;      //cast the int object into an id
  delete (int*)ptr;         //delete the int object like I read online

  id = id - (numberOfProducers + numberOfFaulties);

  // TODO: PROBLEM!!! This will probably only work for 1 consumer thread. Needs to be modified.
  int newItem = -1;

  // Read in the correct number of items from buffer.
  for(int ii = 0; ii < totalNumberOfItems; ii++){

    bool done = false;
    while(!done){

      // If the buffer is not empty, read and process
      if(!buffer.bufferIsEmpty()){

        int item = buffer.removeItem();
        bool isPrime = isIntegerPrime(newItem);

        if(isPrime){
          //printf("Consumer thread: %i is prime!\n", item);
        } else {
          //printf("Consumer thread: %i is not prime.\n", item);
        }

      } else {
        done = false;
      }

    }

  } // End for loop

  pthread_exit(0);
}

void *fillBuffer(void *ptr) {
  int randomNumber;
  int id = *(int*)ptr;      //cast the int object into an id
  delete (int*)ptr;         //delete the int object like I read online

  //use mutex to make sure only one producer is working at a time.
  while(true) {
    pthread_mutex_lock( &buffer_mutex );
    randomNumber = rand() % 999999 + 2;
    pthread_mutex_unlock( &buffer_mutex );
  }

  pthread_exit(0);
}

bool isIntegerPrime(int toTest){
  // Function to test the primality of a number.
  // Reference: http://www.codeproject.com/Articles/465041/Primality-Test

  // If less than 2, the number cannot be prime.
  if(toTest < 2) return false;
  if(toTest == 2) return true;

  // Even numbers cannot be prime
  if(toTest % 2 == 0) return false;

  for(int i = 3; i <= sqrt(toTest); i += 2){
        if(toTest %i == 0)
            return false;
    }

    return true;
}

bool isIntegerEven(int toTest){
  if(toTest % 2 == 0){
    return true;
  } else {
    return false;
  }
}
