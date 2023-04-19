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
    int customers_inside;
    int line_count;
    int customerID;
    int workerID;
    int moleculeID;
} postInfo_t;

// struct LinkedList {
//   struct Node* head;
//   int size;
// };

// struct Node {
//   int data;
//   struct Node* next;
// };