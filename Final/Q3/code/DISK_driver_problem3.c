//    Final Project COMP 310
//    Your Name: NAMDAR KABOLINEJAD
//    Your McGill ID: 260893536

#include <stdlib.h>
#include "DISK_driver.h"
#include "DISK_driver_problem1.h"
#include "DISK_driver_problem2.h"
#include "DISK_driver_problem3.h"

int countTotalChars(int fatIndex);

/*
activeIndex = index of the active_file_table pointer to seek
offset = number of chars to chage, + or -

Returns:
0 on sucess
-1 on error
*/
int seek_EXAM_function(int activeIndex, int offset){

	// check if we have a file pointer at the index given
	if(active_file_table[activeIndex] == NULL){
		printf("ERROR: Index number not in use.\n");
		return -1;
	}

	// find the fat table index for the activeFile
	int fatIndex = -1;
	for(int i=0; i<FAT_SIZE; i++){
		if(fat[i].ftIndex == activeIndex){
			fatIndex = i;
		}
	}

	// safeguard in case not fat entery has the activeFile
	if(fatIndex == -1) return -1;

	// calculate the number of chars we can seek
	int charsForward = (countTotalChars(activeIndex) - fat[fatIndex].byteIndex);
	int charsBackwards = (-1)*fat[fatIndex].byteIndex;

	// get file pointer
	FILE *fp = active_file_table[activeIndex];

	// check if the seek is within bounds
	if(offset > 0 && offset > charsForward){
		printf("ERROR: Out of bounds. Stopped at end of file.\n");

		// seek to the end of the file
		fseek(fp, sizeof(char)*(charsForward), SEEK_CUR);

		// update FAT info
		fat[fatIndex].byteIndex += charsForward;
		if(fat[fatIndex].file_length == 0){
			fat[fatIndex].current_location = 0;
		}
		else {
			fat[fatIndex].current_location = fat[fatIndex].file_length-1;
		}
	}
	else if(offset < 0 && offset < charsBackwards){
		printf("ERROR: Out of bounds. Stopped at start of file.\n");
		fseek(fp, sizeof(char)*charsBackwards-1, SEEK_CUR);

		// update FAT info
		fat[fatIndex].byteIndex += charsBackwards;
		fat[fatIndex].current_location = 0;
	}
	else{
		if(offset < 0) offset -= 1;
		
		fseek(fp, sizeof(char)*offset, SEEK_CUR);

		// update FAT info
		fat[fatIndex].byteIndex += offset;
		fat[fatIndex].current_location += offset/partition.block_size;
	}

	charsForward = (countTotalChars(activeIndex) - fat[fatIndex].byteIndex);
	charsBackwards = (-1)*fat[fatIndex].byteIndex;

	return 0;
}


/*
return the total number of chars in a file
fatIndex = the index of the file in the fat table
*/
int countTotalChars(int fatIndex){

	// the mazimum number of chars the file can have
	int maxSize = fat[fatIndex].file_length * partition.block_size;
	int lastBlock = -1;

	for(int i = 0; i< 10; i++){
		if(fat[fatIndex].pointers[i] == -1){
			lastBlock = fat[fatIndex].pointers[i-1];
			break;
		}
	}

	// check the number of chars in the last block
	char filePath[100];
	sprintf(filePath, "./PARTITION/%s", partition.name);
	FILE *fp = fopen(filePath, "r");
	if (fp == NULL) return -1;

	// seek to the last block belonging to the file
	fseek(fp,
		sizeof(partition)
		+sizeof(fat)
		+sizeof(char)*lastBlock*partition.block_size, SEEK_SET);

	int i = 0;

	for(; i<partition.block_size; i++){
		char ch = fgetc(fp);
		if(ch == '0') break;
	}

	fclose(fp);

	return (maxSize - partition.block_size + i);
}
