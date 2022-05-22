#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ram.h"
#include "memorymanager.h"
#include "pcb.h"

/*
This function will delete the frames in ram for a given PCB
*/
void removeFromRam (PCB* pcb){
    // free the ram frames of the pcb 
    for (int i=0; i<10; i++){
        // printf("At Step %d ram: \n", i);
        // printTracker();
        // printf("\n\n");

        int frameIndex = pcb->pageTable[i];
        if(frameIndex != -1){
            frameIndex = frameIndex*4;
            for(int j=0; j<4; j++){
                ram[frameIndex+j] = NULL;
            }
        }
    }

    // free the inner tracker item for the PCB
    for(int i=0; i<3; i++){
        if((innerTracker[i] != NULL) && (((innerTracker[i]->pcb)-> PC) == (pcb->PC))){
            free(innerTracker[i]);
            innerTracker[i] = NULL;
            break;
        }
    }
}

void printRam(){
    printf("RAM State: \n");
    for(int i=0; i<RAM_SIZE; i++){
        if(ram[i] != NULL) printf("%d\t-->\t%s\n", i, ram[i]);
        else printf("%d\t-->\tNULL \n", i);
    }
}