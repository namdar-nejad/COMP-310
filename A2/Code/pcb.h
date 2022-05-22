#ifndef PCB_H
#define PCB_H

typedef struct PCB{
	int PC;
	int start;
	int end;
} PCB;

PCB* makePCB(int start, int end);

#endif //PCB_H