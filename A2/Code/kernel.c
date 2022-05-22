#include "shell.h"
#include "ram.h"
#include "pcb.h"
#include "cpu.h"
#include "kernel.h"

#include <stdio.h>
#include <stdlib.h>

#define quant 2

int main(int argc, char *argv[]){
	ram_initialize();
	cpu_initialize();

	shellUI();
	
	return 0;
}

int myinit(char *filename){

	PCB* pcb; // create PCB
	if(addToRam(filename) == 1){
		endAllPCB();
		return 1;
	}

	// update the pc value of the PCB
	pcb = makePCB(getStart(filename), getEnd(filename));
	addToReady(pcb);
	return 0;
}

void addToReady(struct PCB * pcb){
	// new queue
	if(head == NULL){
		head = (qNode*)malloc(sizeof(qNode));
		head->val = pcb;
		head->next = NULL;
		tail = head;
	}
	else{ // not new
		qNode * temp = (qNode*)malloc(sizeof(qNode));
		temp->val = pcb;
		temp->next = NULL;
		tail->next = temp;
		tail = temp;
	}
}

int scheduler(){
	// empty CPU
	while(head != NULL){
			int schError = 0;
			core->quanta = quant;
			core->IP = (head -> val) -> PC;
			int lineLeft = ((head -> val) -> end)-(core->IP)+1;
			if( lineLeft >= quant){
				schError = runPCB(quant);
				if(schError == 1){
					endAllPCB();
					formatRam();
					return 1;
				}
				else{
					rotaitPCB();
				}
			}
			else{
				schError = runPCB(lineLeft);
				if(schError == 1){
					endAllPCB();
					formatRam();
					return 1;
				}
				else{
					endPCB();
				}
			}
		}
	return 0;
}

void rotaitPCB(){
	(head->val)->PC = core->IP;
	tail->next = head;
	tail = tail->next;
	head = head->next;
	tail->next = NULL;
}

void endAllPCB(){
	while(head != NULL){
		int start = (head->val)->start;
		int end = (head->val)->end;
		clearSpace(start, end);
		struct qNode *tmp = head->next;
		head->next = NULL;
		free(head);
		head = tmp;
	}
}

void endPCB(){
	int start = (head->val)->start;
	int end = (head->val)->end;
	clearSpace(start, end);
	struct qNode *tmp = head->next;
	head->next = NULL;
	free(head);
	head = tmp;
}
