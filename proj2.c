/**
 * @file proj2.c
 * @name IOS-P2 process synchronization
 * @author Ivan Chodák xchoda00
 * @date 19.4.2023
 * @copyright Copyright (c) 2023
*/
#include "proj2.h"
//TODO
/**
 * Process creation
 * Malloc for customer and Que nodes
*/
int main(int argc, char *argv[]){

    parseArguments(argc, &(*argv));
    
    //set up clean up
    clearSharedMemory();
    unlinkSemaphores();

    //allocates, opens, and setups
    allocateInfoMemory();
    openOutFile();
    initializeSemaphores();
    
    srand((unsigned)time(NULL));

    //finished clean up
    clearSharedMemory();
    unlinkSemaphores();
    return 0;
}
void unlinkSemaphores(){
    sem_unlink("xchoda00.writing");
    sem_unlink("xchoda00.Que1");
    sem_unlink("xchoda00.Que2");
    sem_unlink("xchoda00.Que3");
    return;

}
void initializeSemaphroes(){
    writing = sem_open("xchoda00.writing", O_CREAT | O_EXCL, 0666, 1);
    Que1 = sem_open("xchoda00.Que1", O_CREAT | O_EXCL, 0666, 1);
    Que2 = sem_open("xchoda00.Que2", O_CREAT | O_EXCL, 0666, 1);
    Que3 = sem_open("xchoda00.Que3", O_CREAT | O_EXCL, 0666, 1);
    if(writing == SEM_FAILED || Que1 == SEM_FAILED || Que2 == SEM_FAILED || Que3 == SEM_FAILED){
        exitError("Semaphore failed to open");
    }
    return;
}

void openOutFile(){
    output = fopen("proj2.out", "w+");
    if (output == NULL || ferror(output)){
        exitError("Output file could not be open");
    }
    return;
}

void clearSharedMemory() {
    munmap(postInfo, sizeof(int));
    shm_unlink("/xchoda00.postInfo");

    close(postInfoReturn);
    return;
}

void allocateInfoMemory(){
    
    postInfoReturn = shm_open("/xchoda00.postInfo", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if(postInfoReturn == -1){
        exitError("Memory open failed");
    }
    
    ftruncate(postInfoReturn, sizeof(postInfo_t));

    postInfo = mmap(NULL, sizeof(postInfo_t), PROT_READ | PROT_WRITE, MAP_SHARED, postInfoReturn, 0);
    if(postInfo == MAP_FAILED){
        exitError("Memory mapping failed");
    }
    postInfo->open = true;
    postInfo->notEmpty = 0;
    postInfo->customersInside = 0;
    postInfo->lineCount = 1;
    postInfo->customerID = 1;
    postInfo->workerID = 1;
    // postInfo->Q1 = Que1;
    // postInfo->Q2 = Que2;
    // postInfo->Q3 = Que3; 

    return;
}
void exitError(char *exitMsg){
    fprintf(stderr, exitMsg , "\n");
    fprintf(stderr, "\n");
    exit(1);
}


void parseArguments(int argc, char *argv[]){
    if ( argc != 6){
        exitError("Invalid amount of arguments");
    }
    for (int i = 1; i < argc; i++)
    {
        if(isdigit(*argv[i]) == 0) {
            exitError("Invalid format. Input numbers ");
        }
        
    }
    if(atoi(argv[1]) < 1 || atoi(argv[2]) <1){
        exitError("Cannot pass negative numbers to Arg1 and Arg2");
    }
    if(atoi(argv[3]) < 0 || atoi(argv[4]) < 0 || atoi(argv[5]) < 0 ){
        exitError("Cannot pass negative numbers to Arg3 and Arg4 and Arg5");
    }
    if(atoi(argv[3]) > 10000 || atoi(argv[4]) > 100 || atoi(argv[5]) > 1000){
        exitError("Too big numbers for Arg3(max 10000) or Arg4(max 100) or Arg5(max 1000)");
    }

    customersToCreate = atoi(argv[1]);
    workersToCreate = atoi(argv[2]);
    customerWait = atoi(argv[3]);
    workerBreak = atoi(argv[4]);
    postCloseIn = atoi(argv[5]);
    return;
}

