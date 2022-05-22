#include<string.h>
#include"ram.h"
#include"interpreter.h"
#include"shell.h"
#include"cpu.h"
#include"ram.h"
#include"memorymanager.h"

int run(int quanta){
    // If a quanta greater than the default quanta of 2 is passed,
    // run will instead execute only default quanta of lines.
    if (quanta > DEFAULT_QUANTA ) quanta = DEFAULT_QUANTA;

    for (int i = 0; i < quanta; i++) {
        if(CPU.offset >= 4) return 2;       // page fault, managed by the scheduler

        int index = ((CPU.IP)*4)+(CPU.offset);
        strcpy(CPU.IR,ram[index]);

        int errorCode = parse(CPU.IR);
        
        // REMOVE
        // printCPU();

        // Do error checking and if error, return error
        if (errorCode != 0){
            // Display error message if fatal
            if (errorCode < 0){
                char command[100];
                strcpy(command, CPU.IR);
                command[strlen(command)- 1] = '\0';
                displayCode(errorCode,command);
            }
            return errorCode;
        }
        
        CPU.offset++;
    }
    return 0;
}

// function to print the CPU status 
void printCPU(){
    printf("Current CPU Status: \n");
    printf("IP: %d\n", CPU.IP);
    printf("offset: %d\n", CPU.offset);
    printf("IR: %s\n", CPU.IR);
    printf("quanta: %d\n", CPU.quanta);
}