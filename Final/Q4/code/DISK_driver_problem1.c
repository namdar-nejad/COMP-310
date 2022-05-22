//    Final Project COMP 310
//    Your Name: NAMDAR KABOLINEJAD
//    Your McGill ID: 260893536

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "DISK_driver.h"
#include "DISK_driver_problem1.h"

#define FALSE 0
#define TRUE 1


/*
activeIndex: the index of the active_file_table to fill
name: name of the file to open

Returns:
the index of the fat table with the file name
-1 if the index number is out of bounds
-2 if the index is in use
-3 if there is no empty slot in the FAT Table
-4 on other errors
*/
int open_EXAM_function(int activeIndex, char *name){

	// check if index number is out of bounds (5 is the active table size)
	if(activeIndex < 0 || activeIndex > ACTIVE_SIZE) return -1;

	// check if the active_file_table is empty at the given index
	if(active_file_table[activeIndex] != NULL){
		printf("ERROR: Index number in use.\n");
		return -2;
	}

	// checking the FAT table to see if the file is open, close it and reset indices if it is open
	int found = FALSE;		// whether or not the file is already in the FAT
	int fatIndex = 0;
	for(; fatIndex < FAT_SIZE; fatIndex++){

		// file is in the FAT table
		if((fat[fatIndex].filename != NULL) && (strcmp(fat[fatIndex].filename, name) == 0)){

			// close current file pointer for the file in the active_file_table and free the old pipeline buffer for the file
			if(fat[fatIndex].ftIndex != -1){
				fclose(active_file_table[fat[fatIndex].ftIndex]);
				
				// NEW:
				free(pipeArr[fatIndex]);
				// END;
			}

			// reset file table index index
			fat[fatIndex].ftIndex = -1;

			// reset the active file table entery
			active_file_table[fat[fatIndex].ftIndex] = NULL;

			// the file is already in FAT
			found = TRUE;

			break;
		}
	}

	// find the FAT index of the file that currently has an open pointer the file table slot we want
	int oldFileIndex = 0;
	for(; oldFileIndex<FAT_SIZE; oldFileIndex++){
		if(fat[oldFileIndex].ftIndex == activeIndex) break;
	}

	// path of file in PARTITION
	char filePath[100];
	sprintf(filePath, "./PARTITION/%s", partition.name);

	// pointer to the beginning of the partition data
	FILE *fp = fopen(filePath, "r+");
	fseek(fp, sizeof(partition)+sizeof(fat), SEEK_SET);
	/*
		Note: The file pointer will point to the begining of the data section of the
		partiton and the seeks to the appropriate blocks will happen when doing IO.
		As it is done in the baseline code.
	*/

	// set new values //

	// file already in the FAT
	if(found){

		// update FAT entery's active_file_table index
		fat[fatIndex].ftIndex = activeIndex;

		// update the pointer in active_file_table
		active_file_table[activeIndex] = fp;

		// set the current location to the begining
		fat[activeIndex].current_location = 0;

		// reset byteIndex to the begining of the block
		fat[activeIndex].byteIndex = 0;

		// NEW:
		// malloc pipe buffer with the max size of chars a file can have
		pipeArr[fatIndex] = malloc(sizeof(char)*partition.block_size*partition.total_blocks);
		// END;

		// reset the file table index of the old file in the index
		fat[oldFileIndex].ftIndex = -1;

		// update partition file with new structure data
		updatePartitonStructs();

		// return FAT slot index
		return fatIndex;
	}
	// file not in FAT, find a FAT slot for file
	else{
		for(int i= 0; i<FAT_SIZE; i++){

			// find free slot in FAT
			if(fat[i].file_length == 0 && fat[i].filename == NULL){

				// allocate FAT entery, set name
				fat[i].filename = strdup(name);

				// update FAT entery's active_file_table index
				fat[i].ftIndex = activeIndex;

				// NEW:
				// malloc pipe buffer with the max size of chars a file can have
				pipeArr[i] = malloc(sizeof(char)*partition.block_size*partition.total_blocks);
				// END;

				// reset the rest of the fat_info, just to be sure we have a fresh slot
				fat[i].file_length = 0;
				fat[i].current_location = 0;
				fat[i].byteIndex = 0;

				for(int j=0; j<10; j++){
					fat[i].pointers[j] = -1;
				}

				// update FAT entery's active_file_table index
				fat[i].ftIndex = activeIndex;

				// update the pointer in active_file_table
				active_file_table[activeIndex] = fp;

				// reset the file table index of the old file in the index
				fat[oldFileIndex].ftIndex = -1;
				
				// update partition file with new structure data
			    updatePartitonStructs();

			    // return FAT slot index
			    return i;
		    }
		}

		// incase there was no empty slots in the FAT table
		return -3;
	}

	// other errors in process
	return -4;
}
