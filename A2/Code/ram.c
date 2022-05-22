#include "shell.h"
#include "ram.h"


#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct endValues{
	char* path;
	int start;
	int end;
} endArray[3];	// Array of current processes in the ram and their info
				// Helps to optimize some parts


void ram_initialize(){
    for (int i = 0; i < RAMSIZE; i++){
        ram[i] = NULL;
    }

    for (int i = 0; i < 3; i++){
        endArray[i].end = -1;
        endArray[i].start = -1;
        endArray[i].path = NULL;
    }
}


int addToRam(char* path){
	char buffer[500];
	FILE *fp = fopen(path,"r");

	if(fp == NULL) {
      printf("Error with file %s \n", path);
      formatRam();
      return 1;
  	}

	int curCell = findCell();
	int start = curCell;

	if(curCell == -1){
		printf("ERROR: Not enough RAM to add program.\n");
		formatRam();
		fclose(fp);
		return 1;
	}

	// finding an empty space in the tracker
	// and loding process the info (start, path) in it
	int found = -1;
	for(int i=0; i<3; i++){
		if(endArray[i].start == -1){
			endArray[i].path = strdup(path);
			endArray[i].start = start;
			found = 0;
			break;
		}
	}

	if(found == -1){
		printf("Can't have more than 3 process.\n");
		formatRam();
		fclose(fp);
		return 1;
	}

	while(fgets (buffer, 500, fp)!=NULL){
		ram[curCell++] = strdup(buffer);
		if(curCell >= RAMSIZE){
			printf("ERROR: Not enough RAM to add program.\n");
			formatRam();
			fclose(fp);
			return 1;
		}
	}

	// finding the processes cell in the tracker
	// loding process the info (end) in it
	for(int i=0; i<3; i++){
		if(strcmp(endArray[i].path, path) == 0){
			endArray[i].end = (curCell-1);
			break;
		}
	}

	fclose(fp);
	return 0;
}


// Private Functions
void clearSpace(int start, int end){
	if(start == end) ram[start] = NULL;
	else{
		for(int i=start; i<end; i++){
			ram[i] = NULL;
		}
	}
	// freeing the processes tracker cell
	for(int i=0; i<3; i++){
		if(endArray[i].start == start){
			endArray[i].start = -1;
			endArray[i].path = NULL;
			endArray[i].end = -1;
			break;
		}
	}
}

void formatRam(){
	for(int i=0; i<1000; i++){
		ram[i] = NULL;
	}

	// freeing the processes tracker cell
	for(int i=0; i<3; i++){
		endArray[i].start = -1;
		endArray[i].path = NULL;
		endArray[i].end = -1;
	}
}

// gives first empty ram array cell
int findCell(){
	int curCell = -1;
	for(int i=0; i<RAMSIZE; i++){
		if(ram[i] == NULL){
			curCell = i;
			break;
		}
	}
	return curCell;
}

int getStart(char * fileName){
	for(int i=0; i<3; i++){
		if(strcmp((endArray[i].path), fileName) == 0) return endArray[i].start;
	}
	return -1;
}

int getEnd(char * fileName){
	for(int i=0; i<3; i++){
		if(strcmp((endArray[i].path), fileName) == 0) return endArray[i].end;
	}
	return -1;
}