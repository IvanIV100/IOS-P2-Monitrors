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
    printf("Entering main \n");
    parseArguments(argc, &(*argv));
    
    //person_t person;
    //set up clean up
    printf("set up\n");
    clearSharedMemory();
    closeSemaphores();
    printf("cleaned up \n");

    //allocates, opens, and setups
    printf("mem alloc\n");
    allocateMemory();
    printf("open file\n");
    openOutFile();
    printf("open sem \n");
    initializeSemaphores();

    for (int workerID = 1; workerID <= workersToCreate; workerID++){
        printf("creating worker %d \n", workerID);
        createWorker(workerID);
    }

    for (int customerID = 1; customerID <= customersToCreate; customerID++){
        printf("creating customer %d \n", customerID);
        createCustomer(customerID);

    }
    

    int allBeings = customersToCreate + workersToCreate;

    for (int currentOne = 1; currentOne <= allBeings; currentOne++){
        printf("creating person %d \n", currentOne);
        sem_post(spawn);
    }
    printf("go sleep \n");
    goToSleep(postCloseIn);
    printf("done sleep \n");
    sem_wait(writing);
    fprintf(output, "%d: closing\n", ++postInfo->lineCount);
    sem_post(writing);
    postInfo->open = false;
    //finished clean up
    printf("wait to close \n");
    while (wait(NULL) > 0) {
        continue;
    }
    printf("all closed\n");
    closeSemaphores();
    clearSharedMemory();
    fclose(output);
    output = NULL;
    exit(0);
}

int randomValue(int valueTop){
    srand((unsigned)time(NULL));
    return (rand() % valueTop);
}

void goToSleep(int miliseconds){
    if(miliseconds == 0)
        return;
    int waitFor = randomValue(miliseconds*1000);
    usleep(waitFor); 
    return;
}

void createCustomer(int customerId){
    pid_t customerPid = fork();
    if (customerPid < 0){
        exitError("Customer PID creation failed");
    } else if (customerPid == 0){
        printf("waiting for spawn C %d \n", customerId);
        sem_wait(spawn);
        printf("spawned C %d \n", customerId);
        customerExecute(customerId);
        printf("done C %d \n", customerId);
        exit(0);
    } else {
        return;
    }
}

void createWorker(int workerId){
    pid_t workerPid = fork();
    if (workerPid < 0){
        exitError("Customer PID creation failed");
    } else if (workerPid == 0) {
        printf("waiting for spawn U %d \n", workerId);
        sem_wait(spawn);
        printf("spawned U %d \n", workerId);
        workerExecute(workerId);
        printf("done U %d \n", workerId);
        exit(0);
    } else {
        return;
    }
}

void customerExecute(int customerId){
    printf("C ex \n");
    int taskType = randomValue(3) + 1;
    sem_wait(writing);
    fprintf(output, "%d: Z %d: started\n", ++postInfo->lineCount, customerId);
    sem_post(writing);

    goToSleep(customerWait);
    sem_wait(writing);
    if (postInfo->open == false){
        printf("op:false \n");
        fprintf(output, "%d: Z %d: going home\n", ++postInfo->lineCount, customerId);
        sem_post(writing);
        exit(0);
    } 
    sem_post(writing);
    printf("before entry\n");

    if(postInfo->open == true){
        printf("op:true \n");
        
        sem_wait(writing);
        ++postInfo->customersInside;
        postInfo->isEmpty = false;
        fprintf(output, "%d: Z %d: entering office for a service %d\n", ++postInfo->lineCount, customerId, taskType);
        sem_post(writing);
    
        switch (taskType) {
            case 1:
                sem_wait(writing);
                postInfo->boolCount[taskType-1] = 1;
                sem_post(writing);
                sem_wait(Que1);
                sem_wait(writing);
                fprintf(output, "%d: Z %d: called by office worker\n", ++postInfo->lineCount, customerId);
                sem_post(writing);
                
                goToSleep(randomValue(10));

                sem_wait(writing);
                fprintf(output, "%d: Z %d: going home\n", ++postInfo->lineCount, customerId);
                sem_post(writing);
                sem_wait(writing);
                --postInfo->customersInside;
                printf("current inside: %d \n",postInfo->customersInside);
                sem_post(writing);
                
                exit(0);
                break;
            case 2:
                sem_wait(writing);
                postInfo->boolCount[taskType-1] = 1;
                sem_post(writing);
                sem_wait(Que2);
                sem_wait(writing);
                fprintf(output, "%d: Z %d: called by office worker\n", ++postInfo->lineCount, customerId);
                sem_post(writing);

                goToSleep(randomValue(10));

                sem_wait(writing);
                printf("current inside: %d \n",postInfo->customersInside);
                fprintf(output, "%d: Z %d: going home\n", ++postInfo->lineCount, customerId);
                sem_post(writing);
                sem_wait(writing);
                --postInfo->customersInside;
                printf("current inside: %d \n",postInfo->customersInside);
                sem_post(writing);
                
                exit(0);
                break;

            case 3:
                sem_wait(writing);
                postInfo->boolCount[taskType-1] = 1;
                sem_post(writing);
                sem_wait(Que3);
                sem_wait(writing);
                fprintf(output, "%d: Z %d: called by office worker\n", ++postInfo->lineCount, customerId);
                sem_post(writing);

                goToSleep(randomValue(10));

                sem_wait(writing);
                printf("current inside: %d \n",postInfo->customersInside);
                fprintf(output, "%d: Z %d: going home\n", ++postInfo->lineCount, customerId);
                sem_post(writing);
                sem_wait(writing);
                --postInfo->customersInside;
                printf("current inside: %d \n",postInfo->customersInside);
                sem_post(writing);
                
                exit(0);
                break;

            default:
                
                exitError("Internal error while assigning service type");
                break;
            }
    } else {
        printf("ret out\n");
        return;
    }
    printf("ret out\n");
    return;
}

void workerExecute(int workerId){
        printf("W ex \n");
        sem_wait(writing);
        fprintf(output, "%d: U %d: started\n", ++postInfo->lineCount, workerId);
        sem_post(writing);
            
        while(1){
            if (postInfo->customersInside <= 0){
                postInfo->isEmpty = true;
            }
            if (postInfo->isEmpty == true){
                //printf("empty \n");
                if (postInfo->open == false && postInfo->isEmpty == true){
                    printf("closed \n");
                    sem_wait(writing);
                    fprintf(output, "%d: U %d: going home\n", ++postInfo->lineCount, workerId);
                    sem_post(writing);
                    return;
                } else {
                    sem_wait(writing);
                    fprintf(output, "%d: U %d: taking break\n", ++postInfo->lineCount, workerId);
                    sem_post(writing);
                    goToSleep(randomValue(workerBreak));
                    sem_wait(writing);
                    fprintf(output, "%d: U %d: break finished\n", ++postInfo->lineCount, workerId);
                    sem_post(writing);

                    continue;
                }
            } else {
                int taskType = (rand() % 3);
                while(postInfo->boolCount[taskType] == 0){
                    taskType = (rand() % 3);
                    if (postInfo->boolCount[0] == 0 && postInfo->boolCount[1] == 0 && postInfo->boolCount[2] == 0 ){
                        continue;
                    }
                }
                taskType++;
                switch (taskType) {
                    case 1:
                        //printf("1 W\n");
                        sem_wait(writing);
                        fprintf(output, "%d: U %d: serving a service of type %d\n", ++postInfo->lineCount, workerId, taskType);
                        sem_post(writing);
                        sem_post(Que1);
                        goToSleep(randomValue(10));

                        sem_wait(writing);
                        fprintf(output, "%d: U %d: service finished\n", ++postInfo->lineCount, workerId);
                        sem_post(writing);

                        break;

                    case 2:
                        //printf("2 W\n");
                        sem_wait(writing);
                        fprintf(output, "%d: U %d: serving a service of type %d\n", ++postInfo->lineCount, workerId, taskType);
                        sem_post(writing);

                        sem_post(Que2);
                        goToSleep(randomValue(10));

                        sem_wait(writing);
                        fprintf(output, "%d: U %d: service finished\n", ++postInfo->lineCount, workerId);
                        sem_post(writing);
                        break;

                    case 3:
                        //printf("1 W\n");
                        sem_wait(writing);
                        fprintf(output, "%d: U %d: serving a service of type %d\n", ++postInfo->lineCount, workerId, taskType);
                        sem_post(writing);
                        
                        sem_post(Que3);
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
    sem_unlink("xchoda00.writing");
    sem_unlink("xchoda00.spawn");
    sem_unlink("xchoda00.Que1");
    sem_unlink("xchoda00.Que2");
    sem_unlink("xchoda00.Que3");
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
    postInfo->boolCount[0] = 0;
    postInfo->boolCount[1] = 0;
    postInfo->boolCount[2] = 0;
    // postInfo->customerID = 0;
    // postInfo->workerID = 0;
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

