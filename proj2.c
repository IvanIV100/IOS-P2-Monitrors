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

    

    return 0;
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
    if(atoi(argv[3]) > 10000 || atoi(argv[4]) > 100 || atoi(argv[5] > 1000)){
        exitError("Too big numbers for Arg3(max 10000) or Arg4(max 100) or Arg5(max 1000)");
    }
    return;
}

