#ifndef MEMORYM_H
#define MEMORYM_H

#define TRUE 1
#define FALSE 0

#include"pcb.h"
#include <stdio.h>

// Struct used to store the pcbs and fileNum of each file input to exec
// it is used to optimize some processes in the code
// an ulternative way would be to add extra feilds to the pcb struct which
// I'm not sure we are allowed to do
typedef struct TrackerItem {
	int fileNum;
	PCB* pcb;
} TrackerItem;

// Array to keep track of the three files given to exec 
TrackerItem* innerTracker[3];

void initTracker();
void printTracker();
int launcher(char* filePath, int fileNum);
int findFrame();
int findVictim(PCB *p);
void updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame);
void loadPage(int pageNumber, FILE *f, int frameNumber);
int countTotalPages(FILE *f);
int frameBelongs(PCB *p, int frameNum);
void removeFrame(int frameNum);
int beingUsed(int frameNum);

#endif //MEMORYM_H