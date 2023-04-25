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
void closeSemaphores();
void workerExecute(int workerID);
void customerExecute(int customerID);
void goToSleep();
int randomValue();
void createWorker(int workerID);
void createCustomer(int customerID);
void customerTask(int customerID);

typedef struct postInfoShared {
    bool open;
    bool isEmpty;
    int customersInside;
    int lineCount;
    int boolCount[3];

    //int customerID;
    //wint workerID;
    // struct Que* Q1;
    // struct Que* Q2;
    // struct Que* Q3; 
} postInfoShared_t;

// typedef struct person {
//     int currentID;
//     int taskType;
    
// } person_t;




int customersToCreate;
int workersToCreate;
int customerWait;
int workerBreak;
int postCloseIn;

postInfoShared_t *postInfo;
int postInfoReturn;

sem_t *writing;
sem_t *spawn;
sem_t *spawner;
sem_t *Que1;
sem_t *Que2;
sem_t *Que3;




FILE *output;

