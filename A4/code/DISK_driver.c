#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include "DISK_driver.h"

#define FAT_SIZE 20

struct PARTITION {
	int total_blocks;
	int block_size;
	char* name;
} partition;

struct FAT {
    char *filename;
    int file_length;
    // replaced the array with a pointer to support non-contigous allocation
    int *pointers;
    int current_location;
    int ftIndex;
} fat[FAT_SIZE];

char * block_buffer;

FILE *active_file_table[5];

// helper functions
void clearBlocks(int fileIndex, char buffer[]);
void resetPointer(FILE* fp, int position);
void updatePartiton(char buffer[]);
int findFileTableIndex();
int loadFileTable(int tableIndex, int fatIndex);
void updatePartitonStructs();


void initIO(){
	// initilize the Partition values
	partition.total_blocks = 0;
  	partition.block_size = 0;
  	partition.name = NULL;

  	// initilize the FAT Table enteries
	for(int i=0; i<FAT_SIZE; i++){
		fat[i].filename = NULL;
		fat[i].file_length = 0;
		fat[i].current_location = 0;
		fat[i].pointers = NULL;
		fat[i].ftIndex = -1;
	}

	block_buffer = NULL;

	// initilize the active_file_table enteries
	for(int i=0 ; i<5; i++){
		active_file_table[i] = NULL;
	}
}

int partitionMethod(char *name, int blocksize, int totalblocks){
	// clean the data structures
	initIO();

	// set partition values
	partition.total_blocks = totalblocks;
  	partition.block_size = blocksize;
  	partition.name = strdup(name);

  	block_buffer = malloc(sizeof(partition.block_size));

  	// initilize the pointer array of the FAT Table
	for(int i=0; i<FAT_SIZE; i++){
  		fat[i].pointers = (int*)malloc(sizeof(int)*partition.total_blocks);
  		for(int j=0; j<partition.total_blocks; j++) *(fat[i].pointers+j) = -1;
	}

  	// create the file path in the PARTITION dir
	if(system("mkdir PARTITION > /dev/null 2>&1") == -1) return 0;

	// open file to write to it
	char filePath[100];
	sprintf(filePath, "./PARTITION/%s", name);
	FILE *fp = fopen(filePath, "w");
  	if(fp == NULL) return 0;

  	// write to the file
  	int fwriteState = 1;
  	fwriteState = fwrite (&partition, sizeof(partition), 1, fp);
  	if(fwriteState == 0) return 0;
  	for(int i =0; i<FAT_SIZE; i++){
  		fwriteState = fwrite (&fat[i], sizeof(fat[i]), 1, fp);
  		if(fwriteState == 0) return 0;
  	}

  	// initilizing the partition data
  	for(int i = 0; i < partition.total_blocks * partition.block_size; i++){
    	fputc('*',fp);
  	}

  	// close file
  	fclose(fp);
	
	return 1;
}

int mountFS(char *name){
	// create the relative file path in the PARTITION dir
	char filePath[100];
	sprintf(filePath, "./PARTITION/%s", name);
	FILE *f = fopen(filePath, "r");
  	if(f == NULL) return 0;

  	// clear the active_file_table enteries
	for(int i=0 ; i<5; i++){
		active_file_table[i] = NULL;
	}
  	// read the Partition data
  	fread(&(partition), sizeof(partition),1,f);
  	// read FAT table data
  	for(int i =0; i<FAT_SIZE; i++){
		fread(&fat[i], sizeof(fat[i]), 1, f);
	}
	// update the block_buffer
	block_buffer = malloc(sizeof(partition.block_size * partition.total_blocks));

  	fclose(f);
  	return 1;
}

int openfile(char *name){
	// check if we have a partition in place
	if(partition.total_blocks == 0 && partition.block_size == 0) return -1; 

	// checking the FAT table
	for(int i=0; i<FAT_SIZE; i++){
		if((fat[i].filename != NULL) && (strcmp(fat[i].filename, name)== 0)){
			// add to the Active File Table
			if((fat[i].ftIndex != -1) && (active_file_table[fat[i].ftIndex] != NULL)) return i;
			else if(loadFileTable(findFileTableIndex(), i) != -1) return i;
			else return -1;
		}
	}

	// file not in FAT
	for(int i= 0; i<FAT_SIZE; i++){
		if(fat[i].file_length == 0){
		    if(findFileTableIndex() != -1){
		    	if(loadFileTable(findFileTableIndex(), i) != -1){
		    		fat[i].filename = strdup(name);
		    		updatePartitonStructs();
		    		return i;
		    	}
		    }
		    else{
		    	printf("no space on file table.\n");
		    	return -1;
		    }
	    }
	}

	return -1;
}

int writeBlock(int fileIndex, char *data){

	// problem with openFile
	if(fileIndex == -1) return 0;

	// check if we have enought space in the file
	int numBlocksNeeded = (int) ceil((double) (strlen(data)/(double)partition.block_size));
	int numBlockLeft = partition.total_blocks;
	for(int i = 0; i<FAT_SIZE; i++){
		if(i != fileIndex) numBlockLeft -= fat[i].file_length;
 	}
	if(numBlocksNeeded > numBlockLeft) return 0;

	// copy all the data to a buffer
	FILE *fp = active_file_table[fat[fileIndex].ftIndex];
	resetPointer(fp, 0);
	char* buffer = (char*) malloc(sizeof(char)*(partition.total_blocks*partition.block_size));
	int i = 0;
	for(; i < partition.total_blocks * partition.block_size; i++){
    	buffer[i] = fgetc(fp);
  	}
  	buffer[i] = '\0';
  	
  	// clear the old blocks of the file
  	clearBlocks(fileIndex, buffer);

  	// calculate the blocks to write to
  	int newBlockIndexes[partition.total_blocks];
  	memset(newBlockIndexes, (-1),sizeof(newBlockIndexes));

  	int j=0;
  	for(int i =0; i <numBlocksNeeded ; i++){
  		int index = -1;
  		for(; j<partition.total_blocks; j++){
  			if(buffer[j*partition.block_size] == '*'){
  				index = j;
  				j++;
  				break;
  			}
  		}
  		if(index == -1) return 0;
  		newBlockIndexes[i] = index;
  	}

  	// write new blocks
  	for(int i=0; i<numBlocksNeeded; i++){
  		for(int j=0; j<partition.block_size; j++){
  			if(data[i*partition.block_size+j] == '\0') break;
  			buffer[newBlockIndexes[i]*partition.block_size+j] = data[i*partition.block_size+j];
  		}
  	}

  	// update the fat values
  	fat[fileIndex].file_length = numBlocksNeeded;
  	memcpy(fat[fileIndex].pointers, newBlockIndexes, sizeof(int)*partition.total_blocks);
  	fat[fileIndex].current_location = *fat[fileIndex].pointers;

  	// write new data back to the file
	updatePartiton(buffer);
	free(buffer);

  	return 1;
}

/*
returns the value of a file in the block_buffer
*/
int readBlock(int fileIndex){

	// fopen problem
	if(fileIndex == -1) return 0;

	// create a new buffer and fill with the data
	char buffer[partition.total_blocks * partition.block_size];
	FILE *fp = active_file_table[fat[fileIndex].ftIndex];
	resetPointer(fp, 0);
	int i = 0;
	for(; i < partition.total_blocks * partition.block_size; i++){
		char ch = fgetc(fp);
    	buffer[i] = ch;
  	}
  	buffer[i] = '\0';

  	// buffer to return
	char data[(fat[fileIndex].file_length)*(partition.block_size)+1];
	memset(data, '\0',sizeof(data));

	// collect data from the blocks
	fat[fileIndex].current_location = 0;
  	for(; fat[fileIndex].current_location<fat[fileIndex].file_length; fat[fileIndex].current_location++){
  		int blockNum = *(fat[fileIndex].pointers+fat[fileIndex].current_location);
  		for(int j=0; j<partition.block_size; j++){
  			char ch = buffer[blockNum*partition.block_size+j];
  			if(ch == '*') break;
  			data[fat[fileIndex].current_location*partition.block_size+j] = ch;
  		}
  	}

  	block_buffer = strdup(data);
  	fat[fileIndex].current_location = 0;

  	return 1;
}

// helper functions //

/*
clears the blocks of a file
*/
void clearBlocks(int fileIndex, char buffer[]){
  	for(int i=0; i<fat[fileIndex].file_length;i++){
  		int blockIndex = *((fat[fileIndex].pointers)+i);
  		if(blockIndex == -1) break;
  		for(int j=0; j<partition.block_size; j++){
  			buffer[blockIndex*partition.block_size+j] = '*';
  		}
  	}
}

/*
loads a file into the file table 
*/
int loadFileTable(int tableIndex, int fatIndex){
	if(tableIndex < 0 || fatIndex < 0) return -1;

	char filePath[100];
	sprintf(filePath, "./PARTITION/%s", partition.name);
	FILE *fp = fopen(filePath, "r+");
  	if(fp == NULL) return -1;

  	fseek(fp, sizeof(partition)+sizeof(fat), SEEK_SET);

  	fat[fatIndex].ftIndex = tableIndex;
  	active_file_table[tableIndex] = (FILE*) malloc(sizeof(fp));
  	active_file_table[tableIndex] = fp;
  	return 0;
}

/*
finds an empty slot in the file table
*/
int findFileTableIndex(){
	for(int j=0; j<5; j++){
		if(active_file_table[j] == NULL) return j;
	}
	return -1;
}

/*
resets the position of a file pointer a block passed to it
*/
void resetPointer(FILE* fp, int position){
	FILE * newfp = fdopen (dup (fileno (fp)), "r+");
	fp = newfp;
	fseek(fp, sizeof(partition)+sizeof(fat)+sizeof(char)*partition.block_size*position, SEEK_SET);
}

/*
returns the block_buffer
*/
char* getBlockBuffer(){
	return block_buffer;
}

/*
prints the data in the partition file
*/
void printFile(){
	char filePath[100];
	sprintf(filePath, "./PARTITION/%s", partition.name);
	FILE *f = fopen(filePath, "r");

   	struct PARTITION outputPart;
   	struct FAT fatOutput;

	fread(&outputPart, sizeof(outputPart), 1, f);

	printf ("total_blocks = %d\tblock_size = %d\n", outputPart.total_blocks, outputPart.block_size);

	for(int i =0; i<FAT_SIZE; i++){
		fread(&fatOutput, sizeof(fatOutput), 1, f);
		printf("Name = %s\tLength = %d\tfileIndex = %d\tcurrentLocation = %d \n", fatOutput.filename, fatOutput.file_length, fatOutput.ftIndex,fatOutput.current_location);
		printf("Pointers:\n");
		for(int j=0; j<partition.total_blocks; j++){
			printf("%d \t",*(fatOutput.pointers+j));
		}
		printf("\n");
	}

	printf("Data:\n");
	for(int i=0; i<partition.block_size*partition.total_blocks; i++){
		char ch = fgetc(f);
		printf("%c", ch);
	}
	printf("\n");

	fclose(f);
}

/*
prints current struct values
*/
void printStructs(){
	printf ("total_blocks = %d\tblock_size = %d\n", partition.total_blocks, partition.block_size);

	for(int i =0; i<FAT_SIZE; i++){
		printf("Name = %s\tLength = %d\tfileIndex = %d\tcurrentLocation = %d \n", fat[i].filename, fat[i].file_length, fat[i].ftIndex,fat[i].current_location);
		printf("Pointers:\n");
		for(int j=0; j<partition.total_blocks; j++){
			printf("%d \t",*(fat[i].pointers+j));
		}
		printf("\n");
	}

	printf("File Table: \n");

	for(int i=0; i<5; i++){
		if(active_file_table[i] == NULL) printf("NULL\n");
		else printf("FILE %d\n", i);
	}
}

/*
Saves the contents of the structs to the partition file
and appends the writes the chars in the buffer
*/
void updatePartiton(char buffer[]){
	if(partition.name != NULL){
		char script[100];
		sprintf(script, "cat /dev/null > ./PARTITION/%s", partition.name);
	  	system(script);

		char filePath[100];
		sprintf(filePath, "./PARTITION/%s", partition.name);
		FILE *f = fopen(filePath, "w");

	  	fwrite (&partition, sizeof(partition), 1, f);

	  	for(int i =0; i<FAT_SIZE; i++){
	  		fwrite(&fat[i], sizeof(fat[i]), 1, f);
	  	}

	  	for(int i = 0; i < partition.total_blocks * partition.block_size; i++){
	    	fputc(buffer[i],f);
	  	}

	  	fclose(f);
	}
}

/*
Saves the contents of the structs to the partition file but keeps the old block data
*/
void updatePartitonStructs(){
	if(partition.name != NULL){

		char filePath[100];
		sprintf(filePath, "./PARTITION/%s", partition.name);
		FILE *f = fopen(filePath, "r");

		char buffer[partition.block_size*partition.total_blocks];
		memset(buffer,'\0',sizeof(buffer));

		fseek(f, sizeof(partition)+sizeof(fat), SEEK_SET);

		for(int i=0; i<partition.block_size*partition.total_blocks; i++){
			buffer[i] = fgetc(f);
		}

	  	fclose(f);

	  	updatePartiton(buffer);
	}
}

/*
restes the file table entercies for the FAT table and saves the structs to the partition file
*/
void saveOldPartition(){
	// clean the old file table indexies
	for(int i=0; i<FAT_SIZE; i++){
		fat[i].ftIndex = -1;
	}

	updatePartitonStructs();
}

