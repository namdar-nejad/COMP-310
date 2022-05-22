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