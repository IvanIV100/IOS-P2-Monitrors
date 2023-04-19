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

    //parse args and make basic setup
    parseArguments(argc, &(*argv));
    srand((unsigned)time(NULL));

    //set up clean up
    clearSharedMemory();
    unlinkSemaphores();

    //allocates, opens, and setups
    allocateMemory();
    openOutFile();
    initializeSemaphores();

    //finished clean up
    clearSharedMemory();
    unlinkSemaphores();
    fclose(output);
    output = NULL;
    return 0;
}

int randomValue(int valueTop){
    return (rand() % valueTop);
}

void goToSleep(int miliseconds){
    if(miliseconds == 0)
        return;
    int waitFor = randomValue(miliseconds);
    usleep(waitFor); 
    return;
}

void customerExecute(){
    for (int i = 0; i < customersToCreate; i++){
        pid_t customerPid = fork();
        if (customerPid < 0){
            exitError("Customer PID creation failed");
        }
        
        sem_wait(writing);
        int currentID = ++postInfo->customerID;
        ++postInfo->customersInside;
        postInfo->isEmpty = false;
        int taskType = randomValue(3) + 1;
        fprintf(output, "%d: Z %d: started\n", ++postInfo->lineCount, currentID);
        sem_post(writing);

        goToSleep(customerWait);

        sem_wait(writing);
        if (postInfo->open == false){
            fprintf(output, "%d: Z %d: going home\n", ++postInfo->lineCount, currentID);
            sem_post(writing);
            return;
        } else{
            fprintf(output, "%d: Z %d: entering office for a service %d\n", ++postInfo->lineCount, currentID, taskType);
            sem_wait(writing);
        }
        switch (taskType) {
            case 1:
                sem_wait(Que1);
                sem_wait(writing);
                fprintf(output, "%d: Z %d: called by office worker\n", ++postInfo->lineCount, currentID);
                sem_post(writing);
                goToSleep(randomValue(10));
                sem_wait(writing);
                fprintf(output, "%d: Z %d: going home\n", ++postInfo->lineCount, currentID);
                --postInfo->customersInside;
                if(postInfo->customersInside == 0){
                    postInfo->isEmpty = true;
                }
                sem_post(writing);
                exit(0);
                break;

            case 2:
                sem_wait(Que2);
                sem_wait(writing);
                fprintf(output, "%d: Z %d: called by office worker\n", ++postInfo->lineCount, currentID);
                sem_post(writing);
                goToSleep(randomValue(10));
                sem_wait(writing);
                fprintf(output, "%d: Z %d: going home\n", ++postInfo->lineCount, currentID);
                --postInfo->customersInside;
                if(postInfo->customersInside == 0){
                    postInfo->isEmpty = true;
                }
                sem_post(writing);
                exit(0);
                break;

            case 3:
                sem_wait(Que3);
                sem_wait(writing);
                fprintf(output, "%d: Z %d: called by office worker\n", ++postInfo->lineCount, currentID);
                sem_post(writing);
                goToSleep(randomValue(10));
                sem_wait(writing);
                fprintf(output, "%d: Z %d: going home\n", ++postInfo->lineCount, currentID);
                --postInfo->customersInside;
                if(postInfo->customersInside == 0){
                    postInfo->isEmpty = true;
                }
                sem_post(writing);
                exit(0);
                break;

            default:
                break;
                exitError("Internal error while assigning service type");
        }
    }

}

void workerExecute(){
    for (int i; i < workersToCreate; i++){
        pid_t workerPid = fork();
        if (workerPid < 0){
            exitError("Customer PID creation failed");
        }
        sem_wait(writing);
        int currentID = ++postInfo->workerID;
        fprintf(output, "%d: U %d: started\n", ++postInfo->lineCount, currentID);
        sem_post(writing);

        bool work = true;
        while(work){
            int taskType = (rand() % 3) + 1;
            switch (taskType) {
            case 1:
                sem_wait(writing);
                fprintf(output, "%d: U %d: serving a service of type %d\n", ++postInfo->lineCount, currentID, taskType);
                sem_post(Que1);
                sem_post(writing);
                
                goToSleep(randomValue(10));

                sem_wait(writing);
                fprintf(output, "%d: U %d: service finished\n", ++postInfo->lineCount, currentID);
                sem_post(writing);
                break;

            case 2:
                sem_wait(writing);
                fprintf(output, "%d: U %d: serving a service of type %d\n", ++postInfo->lineCount, currentID, taskType);
                sem_post(Que2);
                sem_post(writing);
                
                goToSleep(randomValue(10));

                sem_wait(writing);
                fprintf(output, "%d: U %d: service finished\n", ++postInfo->lineCount, currentID);
                sem_post(writing);
                break;

            case 3:
                sem_wait(writing);
                fprintf(output, "%d: U %d: serving a service of type %d\n", ++postInfo->lineCount, currentID, taskType);
                sem_post(Que3);
                sem_post(writing);
                
                goToSleep(randomValue(10));

                sem_wait(writing);
                fprintf(output, "%d: U %d: service finished\n", ++postInfo->lineCount, currentID);
                sem_post(writing);
                break;
            default:
                break;
            }
            bool snackTime = false;
            sem_wait(writing);
            if (postInfo->isEmpty == true){
                snackTime = true;
                if (postInfo->open == false){
                    fprintf(output, "%d: U %d: going home\n", ++postInfo->lineCount, currentID);
                    sem_post(writing);
                    exit(0);
                }
            }
            sem_post(writing);
            if (snackTime){
                goToSleep(randomValue(workerBreak));
            }

        }
    
    }
}

void unlinkSemaphores(){

    sem_close(writing);
    sem_close(Que1);
    sem_close(Que2);
    sem_close(Que3);
    
    sem_unlink("xchoda00.writing");
    sem_unlink("xchoda00.Que1");
    sem_unlink("xchoda00.Que2");
    sem_unlink("xchoda00.Que3");
    
    return;
}
void initializeSemaphores(){
    sem_unlink("xchoda00.writing");
    sem_unlink("xchoda00.Que1");
    sem_unlink("xchoda00.Que2");
    sem_unlink("xchoda00.Que3");
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
    setbuf(output, NULL);
    return;
}

void clearSharedMemory() {
    munmap(postInfo, sizeof(int));
    shm_unlink("/xchoda00.postInfo");

    close(postInfoReturn);
    return;
}

void allocateMemory(){
    
    postInfoReturn = shm_open("/xchoda00.postInfo", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if(postInfoReturn == -1){
        exitError("Memory open failed");
    }
    
    ftruncate(postInfoReturn, sizeof(postInfoShared_t));

    postInfo = mmap(NULL, sizeof(postInfoShared_t), PROT_READ | PROT_WRITE, MAP_SHARED, postInfoReturn, 0);
    if(postInfo == MAP_FAILED){
        exitError("Memory mapping failed");
    }
    postInfo->open = true;
    postInfo->isEmpty = true;
    postInfo->customersInside = 0;
    postInfo->lineCount = 0;
    postInfo->customerID = 0;
    postInfo->workerID = 0;
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

