//    Final Project COMP 310
//    Your Name: NAMDAR KABOLINEJAD
//    Your McGill ID: 260893536

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "DISK_driver.h"
#include "DISK_driver_problem1.h"
#include "DISK_driver_problem2.h"

#define TRUE 1
#define FALSE 0

/*
activeIndex = index of the active_file_table to close

Returns:
0 on sucess
-1 on error
*/
int close_EXAM_function(int activeIndex){
	// check if we have a file at that index
	if(active_file_table[activeIndex] == NULL){
		printf("ERROR: Index number not in use.\n");
		return -1;
	}

    // find the fat index
    int fatIndex = 0;
    int found = FALSE;
    for (; fatIndex < FAT_SIZE; fatIndex++){
        if(fat[fatIndex].ftIndex == activeIndex){
            found = TRUE;
            break;
        }
    }

    // not file in FAT with the given active file table index
    if(! found) return -1;

    // close file and reset index
    fclose(active_file_table[activeIndex]);
    active_file_table[activeIndex] = NULL;
    fat[fatIndex].ftIndex = -1;

    // free the pipeline buffer for the file
    // NEW:
    free(pipeArr[fatIndex]);
    // END;
    
    /*
        Note:
        This is all we need to do here.
        The rest of the structures/values are managed in other places.
    */

    // update the Aprtition with the new structure
    updatePartitonStructs();

    return 0;
}

