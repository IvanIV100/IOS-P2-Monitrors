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

typedef struct postInfo {
    bool open;
    int customersInside;
    int lineCount;
    int customerID;
    int workerID;
    struct Que* Q1;
    struct Que* Q2;
    struct Que* Q3;
    
} postInfo_t;

struct Customer {
    int customerID;
    int typeOfAction;
    struct Customer* nextCustomer;
}Customer_t;

struct Que {
    int lenght;
    struct Customer* FirstCustomer;
} Que_t;