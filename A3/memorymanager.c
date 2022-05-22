#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include"memorymanager.h"
#include"ram.h"
#include"kernel.h"
#include"pcb.h"

time_t t;

/*
NOTE:
1. fileNum is used to name the files added to the backingStore
so the first file will be called 1 and so on.

2. I didn't see any point to passing the file pointyer to the lancher()
I just pass the file path and handdle the rest in the function.

3. I have added a innerTracker that is used to optimize some of the 
operations done by the memeory manager. For example, updating the 
page table of a pcb that has been a victim.
*/
int launcher(char* filePath, int fileNum){
	// path of the file in the BackingStore
	// determined by the fileNum passed to the function
	char backingPath[100];
	sprintf(backingPath, "./BackingStore/%d.txt", fileNum);

	// Copying the file to the backing store
	char script[1000];
	sprintf(script, "cp %s %s > /dev/null 2>&1", filePath, backingPath);
	if(system(script) == -1) return 3;
	/*
		Script example:
		cp ./Desktop/1.txt ./BackingStore/1 >& ./ScriptOutput.txt
	*/

    // Openning file in the Backing Storage
	FILE *backFile = fopen(backingPath, "r");
	
	// Check if the BackingStore file is correctly opened
	if(backFile == NULL) return 0;

	// more than 10 pages / 40 lines of code
	if(countTotalPages(backFile) > 10) return 2;

	// Openning file in the Backing Storage
	backFile = fopen(backingPath, "r");
	
	// Check if the BackingStore file is correctly opened
	if(backFile == NULL) return 0;

	// Checking the number of pages in the file
	int numToLoad = countTotalPages(backFile);
	// Creating pcb and adding to the ready queue
	PCB* pcb = makePCB(numToLoad);
	addToReady(pcb);

	if(numToLoad > 2) numToLoad = 2;		// max pages that we are loading is 2

	// Adding to the innerTracker
	TrackerItem* item = (TrackerItem*)malloc(sizeof(TrackerItem));;
	item->fileNum = fileNum;
	item->pcb = pcb;
	innerTracker[fileNum] = item;

	// Adding pages to ram
	for(int i=0; i<numToLoad; i++){
		int curFrame = findFrame();
		int victimFrame = -1;
		if(curFrame == -1){
			victimFrame = findVictim(pcb);

			// deadlock in memory
			// imposible in our case since we only have 3 file and loading one page at a time 
			if(victimFrame == -1) return 0;
		}
		
		backFile = fopen(backingPath, "r");
		if(backFile == NULL) return 0;

		if(curFrame == -1)loadPage(i, backFile, victimFrame);
    	else loadPage(i, backFile, curFrame);
		updatePageTable(pcb, i, curFrame, victimFrame);
	}

	// Setting PC to point to the frame of the first page
	pcb->PC = pcb->pageTable[0];
	return 1;
}

// counts the number of pages in a file (page size = 4)
int countTotalPages(FILE *f){

	int count = 0;
	char chr = getc(f);
	char lastChar;

    while (chr != EOF){
    	lastChar = chr;
        //Count whenever new line is encountered
        if (chr == '\n') count++;
        chr = getc(f);
    }
    // for the last line that doesn't have a \n
    if(lastChar != '\n') count++;

    fclose(f);

    int numPages = -1;
    // Calculating number of pages needed
    if(count%4 == 0) numPages = count/4;
	else numPages = (int) (count + (4-(count%4)))/4;

    return numPages;
}

// counts the number of lines in the last frame of a process
int remaningInLastFrame(PCB* pcb){
    int rtn = 0;
    // checking how many lines we have in the remaning of this frame
    for(int i=(pcb->PC_offset); i<4;i++){
        if(ram[(pcb->PC)*4+i] != NULL) rtn++;
    }
    return rtn;
}

// function to load a page into ram
void loadPage(int pageNumber, FILE *f, int frameNumber){

	int ramSlot = frameNumber*4;
	int pageIndex = pageNumber*4;

	int i = 0;				// # of lines of code copied
	int lineNumber = 0;		// current line of the file
	int reached = FALSE;	// if we have reached the page in the file
	char codeLine[100];		// line of code from the file
	// while the file has lines and we still haven't copied a page
    while ((i < 4) && (fgets(codeLine, sizeof(codeLine), f) != NULL)){
    	// if we have reached the desired page
        if (reached || (lineNumber == pageIndex)) {
        	// copy the line of code to the ram and increment
        	reached = TRUE;
        	ram[ramSlot] = strdup(codeLine);
        	ramSlot++;
            i++;
        }
        else lineNumber++;
    }
    fclose(f);
}

// searches ram to find an empty frame
int findFrame(){
	// checking the first index of each frame on the ram
	// if the first slot of the frame is free the entire frame is free
	for(int i=0; i<10; i++){
		if(ram[i*4] == NULL){
			return i;
		}
	}
	return -1;
}

// function to find a victim frame in the ram
int findVictim(PCB *p){
	srand((unsigned) time(&t));
	int randFrame = rand() % 10;

	// check if the frame is in the page table of the PCB
	// or being used by another PCB
	if(!frameBelongs(p, randFrame)&& (!beingUsed(randFrame))) return randFrame;
	else {
		for(int i=1; i<10; i++){
			randFrame = (randFrame+i)%10;
			if((!frameBelongs(p, randFrame)) && (!beingUsed(randFrame))){
				return randFrame;
			}
		}
	}
	return -1;
}

// check if a frame is curretly being used by a PCB
// true if the PC of any of the PCBs is pointing to that frame
int beingUsed(int frameNum){
	PCB* p;

	for(int i=0; i<3; i++){
		if(innerTracker[i] != NULL){
			p = innerTracker[i]->pcb;
			if((p != NULL)&&(p->PC == frameNum)) return TRUE;
		}
	}

	return FALSE;
}

// check if a frame belongs to a PCB's future page table
// true if the frame is going to be used by the PCB in the future
// false if the frame has been entirly used or not in the pageTable at all
int frameBelongs(PCB *p, int frameNum){
	for(int i=p->PC_page; i<10; i++){
		if(p->pageTable[i] == frameNum) return TRUE;
	}
	return FALSE;
}

// function to update the page table of a PCB
// victim should be -1 if there wasn't a vitim frame
// and the index of the vitim frame if there was
void updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame){
	// didn't have a victim frame, just update the pcb's page table
	if(victimFrame == -1){
		p->pageTable[pageNumber] = frameNumber;
	}
	// had a victim:
	// 1. update the vistims page table by calling removeFrame
	// 2. update the pcb's page table
	else{
		removeFrame(victimFrame);
		p->pageTable[pageNumber] = victimFrame;
	}
}

/*
Uses the innertacker to go through all of the PCB's page tables and set the
frame of a page that has been a vistim to -1.
*/
void removeFrame(int frameNum){
	// for all the items in the innerTracker
	for(int i=0; i<3; i++){
		if(innerTracker[i] != NULL){
			PCB* pcb = innerTracker[i]->pcb;
			// for all the frames in the page table
			for(int j=0; j<10; j++){
				// if the frame is equal to the victim frame number
				if((pcb!=NULL)&&(pcb->pageTable[j] == frameNum)){
					pcb->pageTable[j] = -1;
					break;
				}
			}
		}
	}
}

// used to initilize the tracker
void initTracker(){
	for(int i=0; i<3; i++){
		innerTracker[i] = NULL;
	}
}

// method to print the tracker
void printTracker(){
	printf("Current Tracker State: \n");
    for(int i=0; i<3; i++){
        if(innerTracker[i] != NULL){
        	printf("%d --> (%d): \n", i, innerTracker[i]->fileNum);
        	printPCB(innerTracker[i]->pcb);
        }
        else printf("%d --> NULL \n", i);
    }
}