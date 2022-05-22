#ifndef RAM_H
#define RAM_H

#define RAMSIZE 1000
char *ram[RAMSIZE];

int addToRam(char* path);
void ram_initialize();

int findCell();
void ramError();
void clearSpace(int start, int end);
int getStart(char * fileName);
int getEnd(char * fileName);
void formatRam();

#endif //RAM_H