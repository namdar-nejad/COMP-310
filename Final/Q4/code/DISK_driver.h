#include<stdio.h>

void initIO();
int partitionMethod(char *name, int blocksize, int totalblocks);
int mountFS(char *name);
int openfile(char *name);
int readBlock(int file);
int writeBlock(int file, char *data);

char* getBlockBuffer();
void printStructs();
void printFile();
void saveOldPartition();
int loadFileTable(int tableIndex, int fatIndex);
void updatePartitonStructs();

#define FAT_SIZE 20
#define ACTIVE_SIZE 5

#define TRUE 1
#define FALSE 0

struct PARTITION {
	int total_blocks;
	int block_size;
	char* name;
} partition;

struct FAT {
    char *filename;
    int file_length;
    int pointers[10];
    int current_location;
    int ftIndex;
    int byteIndex;
} fat[FAT_SIZE];

// NEW:
char* pipeArr[FAT_SIZE];
// END;

char * block_buffer;

FILE *active_file_table[ACTIVE_SIZE];
