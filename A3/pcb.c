#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h" 
#include "memorymanager.h"

PCB* makePCB(int pageCount){
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    // current frame index
    pcb->PC = -1;
    memset(pcb->pageTable,-1,sizeof(pcb->pageTable));
    //pcb->pageTable = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    // current page index
    pcb->PC_page = 0;
    // current offset within the frame
    pcb->PC_offset = 0;
    pcb->pages_max = pageCount;
    return pcb;
}

void printPCB(PCB* pcb){
    printf("PCB Info: \n");
    printf("PC (cur frame index): %d\n", pcb->PC);
    printf("PC_page (cur page index): %d\n", pcb->PC_page);
    printf("PC_offset (cur offset): %d\n", pcb->PC_offset);
    printf("pages_max: %d\n", pcb->pages_max);
    printf("Page Table: \n");
    for(int i=0; i<10; i++){
        printf("%d --> %d \n", i, pcb->pageTable[i]);
    }
}

