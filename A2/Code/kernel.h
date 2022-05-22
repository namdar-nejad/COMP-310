//#include <stdlib.h>

#ifndef KERNEL_H
#define KERNEL_H

// Ready Queue Node
typedef struct qNode {
    struct PCB* val;
    struct qNode * next;
} qNode;

// Ready Queue Head & Tail
struct qNode * head;
struct qNode * tail;

void addToReady(struct PCB * pcb);
int myinit(char *filename);
int scheduler();
void endPCB();
void rotaitPCB();
void endAllPCB();

#endif //KERNEL_H