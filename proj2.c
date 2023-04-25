/**
 * @file proj2.c
 * @name IOS-P2 process synchronization
 * @author Ivan Chodák xchoda00
 * @date 19.4.2023
 * @copyright Copyright (c) 2023
*/
#include "proj2.h"

int main(int argc, char *argv[]){
    //maybe dead
    closeSemaphores();
    clearSharedMemory();
    parseArguments(argc, &(*argv));

    allocateMemory();
    openOutFile();
    initializeSemaphores();

    int toSleep = 0;
    if (postCloseIn == 0){
        postInfo->open = false;
    } else {
        toSleep= (randomValue(postCloseIn*1000/2) + postCloseIn*1000/2) ;
    }
    //  else if (postCloseIn == 1){
    //     toSleep = 1000;
    printf("toSleep: %d\n", toSleep );
    
    

    for (int workerID = 1; workerID <= workersToCreate; workerID++){
        pid_t workerPid = fork();
        srand((unsigned)time(NULL));
        if (workerPid < 0){
            exitError("Customer PID creation failed");
        } else if (workerPid == 0) {
            sem_wait(writing);
            fprintf(output, "%d: U %d: started\n", ++postInfo->lineCount, workerID);
            sem_post(writing);
            sem_post(spawn);
            workerExecute(workerID);
            exit(0);
        } else {
            ;
        }
    }
    for (int customerID = 1; customerID <= customersToCreate; customerID++){
        pid_t customerPid = fork();
        srand((unsigned)time(NULL));
        if (customerPid < 0){
            exitError("Customer PID creation failed");
        } else if (customerPid == 0){
            sem_wait(writing);
            fprintf(output, "%d: Z %d: started\n", ++postInfo->lineCount, customerID);
            sem_post(writing);
            sem_post(spawn);
            goToSleep(customerWait);
            customerExecute(customerID);
            exit(0);
        } else {
            ;
        }
    }
    printf("to spawn \n");
    int allBeings = customersToCreate + workersToCreate -2;
    for (int currentOne = 0; currentOne < allBeings; currentOne++){
        sem_wait(spawn);
    }
    
    usleep(toSleep);
    printf("done sleep \n");

    
    sem_wait(writing);
    fprintf(output, "%d: closing\n", ++postInfo->lineCount);
    postInfo->open = false;
    sem_post(writing);
    
    

    while (wait(NULL) > 0) {
        continue;
    }
    printf("done wait \n");
    closeSemaphores();
    clearSharedMemory();
    exit(0);
}

void exitError(char *exitMsg){
    fprintf(stderr, exitMsg , "\n");
    fprintf(stderr, "\n");
    exit(1);
}


int randomValue(int valueTop){
    return (rand() % valueTop);
}

void workerExecute(int workerId){
    while(1){
        sem_wait(writing);
        if (postInfo->isEmpty == true && postInfo->open == false ){
            fprintf(output, "%d: U %d: going home\n", ++postInfo->lineCount, workerId);
            sem_post(writing);
            exit(0);
        } else if (postInfo->isEmpty == true && postInfo->open == true) {
            fprintf(output, "%d: U %d: taking break\n", ++postInfo->lineCount, workerId);
            sem_post(writing);
            goToSleep(randomValue(workerBreak));

            sem_wait(writing);
            fprintf(output, "%d: U %d: break finished\n", ++postInfo->lineCount, workerId);
            sem_post(writing);
            continue;
        } else {
            int taskType = (rand() % 3);
                if (postInfo->boolCount[0] == 0 && postInfo->boolCount[1] == 0 && postInfo->boolCount[2] == 0){
                    printf("postInfoOpen: %d cumsin %d", postInfo->open, postInfo->customersInside);
                    if (postInfo->open == false && postInfo->isEmpty == true) {
                        fprintf(output, "%d: U %d: going home\n", ++postInfo->lineCount, workerId);
                        sem_post(writing);
                        exit(0);
                    } else {
                        fprintf(output, "%d: U %d: taking break\n", ++postInfo->lineCount, workerId);
                        sem_post(writing);
                        goToSleep(randomValue(workerBreak));

                        sem_wait(writing);
                        fprintf(output, "%d: U %d: break finished\n", ++postInfo->lineCount, workerId);
                        sem_post(writing);
                        continue;
                    }
                }
                while (postInfo->boolCount[taskType] == 0)
                {
                    taskType = (rand() % 3);
                }
                sem_post(writing);

                taskType++;
                switch (taskType){
                    case 1:
                        sem_wait(writing);
                        if (postInfo->boolCount[taskType-1] == 0 ){
                            sem_post(writing);
                            continue;
                        }
                        fprintf(output, "%d: U %d: serving a service of type %d\n", ++postInfo->lineCount, workerId, taskType);
                        --postInfo->customersInside;
                        --postInfo->boolCount[taskType-1];
                        if (postInfo->boolCount[0] == 0 && postInfo->boolCount[1] == 0 && postInfo->boolCount[2] == 0){
                            postInfo->isEmpty = true;
                        }
                        sem_post(Que1);
                        sem_post(writing);

                        goToSleep(randomValue(10));

                        sem_wait(writing);
                        fprintf(output, "%d: U %d: service finished\n", ++postInfo->lineCount, workerId);
                        sem_post(writing);
                        break;
                    case 2:
                        sem_wait(writing);
                        if (postInfo->boolCount[taskType-1] == 0 ){
                            sem_post(writing);
                            continue;
                        }
                        fprintf(output, "%d: U %d: serving a service of type %d\n", ++postInfo->lineCount, workerId, taskType);
                        
                        --postInfo->customersInside;
                        --postInfo->boolCount[taskType-1];
                        if (postInfo->boolCount[0] == 0 && postInfo->boolCount[1] == 0 && postInfo->boolCount[2] == 0){
                            postInfo->isEmpty = true;
                        }
                        sem_post(Que2);
                        
                        sem_post(writing);

                        goToSleep(randomValue(10));

                        sem_wait(writing);
                        fprintf(output, "%d: U %d: service finished\n", ++postInfo->lineCount, workerId);
                        sem_post(writing);
                        break;

                    case 3:
                        sem_wait(writing);
                        if (postInfo->boolCount[taskType-1] == 0 ){
                            sem_post(writing);
                            continue;
                        }
                        fprintf(output, "%d: U %d: serving a service of type %d\n", ++postInfo->lineCount, workerId, taskType);
                        --postInfo->customersInside;
                        --postInfo->boolCount[taskType-1];
                        if (postInfo->boolCount[0] == 0 && postInfo->boolCount[1] == 0 && postInfo->boolCount[2] == 0){
                            postInfo->isEmpty = true;
                        }
                        sem_post(Que3);
                        sem_post(writing);

                        goToSleep(randomValue(10));

                        sem_wait(writing);
                        fprintf(output, "%d: U %d: service finished\n", ++postInfo->lineCount, workerId);
                        sem_post(writing);
                        break;
                    default:
                        break;
                }
            }
        }
        
}

void customerExecute(int customerId){
    sem_wait(writing);
    if (postInfo->open == false){
        fprintf(output, "%d: Z %d: going home\n", ++postInfo->lineCount, customerId);
        sem_post(writing);
        exit(0);
    }
    int taskType = randomValue(3) + 1;
    fprintf(output, "%d: Z %d: entering office for a service %d\n", ++postInfo->lineCount, customerId, taskType);
    sem_post(writing);
    ++postInfo->customersInside;
    postInfo->isEmpty = false;
    switch (taskType)
    {
    case 1:
        
        ++postInfo->boolCount[taskType-1];
        sem_wait(Que1);
        customerTask(customerId);
        exit(0);

    case 2:
        ++postInfo->boolCount[taskType-1];
        sem_wait(Que2);
        customerTask(customerId);
        exit(0);

    case 3:
        ++postInfo->boolCount[taskType-1];
        sem_wait(Que3);
        customerTask(customerId);
        exit(0);

    default:
        break;
    }



}

void customerTask(int customerId){
    sem_wait(writing);
    fprintf(output, "%d: Z %d: called by office worker\n", ++postInfo->lineCount, customerId);
    sem_post(writing);
    
    goToSleep(randomValue(10));

    sem_wait(writing);
    fprintf(output, "%d: Z %d: going home\n", ++postInfo->lineCount, customerId);
    sem_post(writing);
    return;
}

void clearSharedMemory() {
    munmap(postInfo, sizeof(int));
    shm_unlink("/xchoda00.postInfo");

    close(postInfoReturn);
    return;
}

void goToSleep(int miliseconds){
    if(miliseconds == 0){
        return;
    }
        
    int waitFor = randomValue(miliseconds*1000);
    usleep(waitFor); 
    return;
}

void closeSemaphores(){

    sem_close(writing);
    sem_close(spawn);
    sem_close(Que1);
    sem_close(Que2);
    sem_close(Que3);
    
    sem_unlink("xchoda00.writing");
    sem_unlink("xchoda00.spawn");
    sem_unlink("xchoda00.Que1");
    sem_unlink("xchoda00.Que2");
    sem_unlink("xchoda00.Que3");
    
    return;
}

void initializeSemaphores(){
    closeSemaphores();
    writing = sem_open("xchoda00.writing", O_CREAT | O_EXCL, 0666, 1);
    spawn = sem_open("xchoda00.spawn", O_CREAT | O_EXCL, 0666, 0);
    Que1 = sem_open("xchoda00.Que1", O_CREAT | O_EXCL, 0666, 0);
    Que2 = sem_open("xchoda00.Que2", O_CREAT | O_EXCL, 0666, 0);
    Que3 = sem_open("xchoda00.Que3", O_CREAT | O_EXCL, 0666, 0);
    if(writing == SEM_FAILED || spawn == SEM_FAILED ||Que1 == SEM_FAILED || Que2 == SEM_FAILED || Que3 == SEM_FAILED){
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
    postInfo->boolCount[0] = 0;
    postInfo->boolCount[1] = 0;
    postInfo->boolCount[2] = 0;

    return;
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
    //check if correct ranges
    if(atoi(argv[1]) < 1 || atoi(argv[2]) <1 || atoi(argv[3]) < 0 || atoi(argv[4]) < 0 || atoi(argv[5]) < 0 ){
        exitError("Cannot pass negative numbers to Arguments[1-5] or 0 to Arg1 and Arg2");
    }
    if(atoi(argv[3]) > 10000 || atoi(argv[4]) > 100 || atoi(argv[5]) > 10000){
        exitError("Too big numbers for Arg3(max 10000) or Arg4(max 100) or Arg5(max 10000)");
    }

    customersToCreate = atoi(argv[1]);
    workersToCreate = atoi(argv[2]);
    customerWait = atoi(argv[3]);
    workerBreak = atoi(argv[4]);
    postCloseIn = atoi(argv[5]);
    return;
}