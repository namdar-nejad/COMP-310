#include "pcb.h"

#include <stdlib.h>


PCB* makePCB(int start, int end){
	PCB* rtn = (PCB*)malloc(sizeof(PCB));
	rtn->PC = start;
	rtn->start = start;
	rtn->end = end;

	return rtn;
}