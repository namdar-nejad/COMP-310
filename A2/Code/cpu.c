#include "cpu.h"
#include "ram.h"
#include "kernel.h"
#include "interpreter.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void cpu_initialize(){
	head = NULL;
	tail = NULL;
	
	core = (struct CPU *)malloc(sizeof(struct CPU));
	core->IP = -1;
}


int runPCB(int time){
	for(int i=0; i<time; i++){
		memset(core->IR, '0', 1000*sizeof(char));
		strcpy(core->IR, ram[core->IP]);
		(core->IP)++;
		int interError = interpret(core->IR);
		if(interError == 1) return 1;
	}
	return 0;
}