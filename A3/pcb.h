#ifndef PCB_H
#define PCB_H

typedef struct PCB
{
    // current frame index
    int PC;

    int pageTable[10];

    // current page index
    int PC_page;

    // current offset within the frame
    int PC_offset;
    
    // max number of pages needed for this process
    int pages_max;

}PCB;


PCB* makePCB(int pageCount);

void printPCB(PCB* pcb);

#endif //PCB_H