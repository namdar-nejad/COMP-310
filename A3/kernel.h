#ifndef KERNEL_H
#define KERNEL_H
#include "pcb.h"

void boot();
int kernel();
void addToReady(struct PCB*);
int size();
struct PCB* pop();
int remaning(PCB* pcb);
int pageIntrupt(PCB* pcb);
void clearBacking();
int scheduler();
void loadNewPages(PCB* p);
int remaningInLastFrame(PCB* pcb);
void emptyReadyQueue();

#endif //KERNEL_H