#ifndef CPU_H
#define CPU_H

struct CPU {
	int IP;
	char IR[1000];
	int quanta;
};

// One instance of the CPU Struct
struct CPU * core;

void cpu_initialize();
int runPCB(int time);

#endif //CPU_H