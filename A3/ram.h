#ifndef RAM_H
#define RAM_H
#include"pcb.h"

#define RAM_SIZE 40
char* ram[RAM_SIZE];

void removeFromRam (PCB*);
void printRam();

#endif //RAM_H