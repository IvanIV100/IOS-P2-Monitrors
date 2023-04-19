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

void exitError(char *exitMsg);
void parseArguments(int argc, char *argv[]);
void allocateMemory();
void clearSharedMemory();
void openOutFile();
void initializeSemaphores();
void unlinkSemaphores();
void workerExecute();
void customerExecute();
void goToSleep();
int randomValue();

typedef struct postInfoShared {
    bool open;
    bool isEmpty;
    int customersInside;
    int lineCount;
    int customerID;
    int workerID;
    // struct Que* Q1;
    // struct Que* Q2;
    // struct Que* Q3; 
} postInfoShared_t;

// typedef struct customer {
//     int customerID;
    
// } customer_t;




int customersToCreate;
int workersToCreate;
int customerWait;
int workerBreak;
int postCloseIn;

postInfoShared_t *postInfo;
int postInfoReturn;

sem_t *writing;
sem_t *Que1;
sem_t *Que2;
sem_t *Que3;




FILE *output;

