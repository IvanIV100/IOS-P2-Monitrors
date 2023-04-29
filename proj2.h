//Included libraries
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>


//Function prototypes
void exitError(char *exitMsg);
void parseArguments(int argc, char *argv[]);
void allocateMemory();
void clearSharedMemory();
void openOutFile();
void initializeSemaphores();
void closeSemaphores();
void cleanUp();
void goToSleep();
int randomValue();

void createWorker(int workerID);
void createCustomer(int customerID);

void workerExecute(int workerID);
void customerExecute(int customerID);
void customerTask(int customerID);

//Shared memory structure
//boolCount is a list that counts the number of customers in each queue
typedef struct postInfoShared {
    bool open;
    bool isEmpty;
    int customersInside;
    int lineCount;
    int boolCount[3];
} postInfoShared_t;

//Shared memory variables
postInfoShared_t *postInfo;
int postInfoReturn;

//Global variables
int customersToCreate;
int workersToCreate;
int customerWait;
int workerBreak;
int postCloseIn;

//Semaphores
sem_t *writing;
sem_t *spawn;
sem_t *Que1;
sem_t *Que2;
sem_t *Que3;
FILE *output;