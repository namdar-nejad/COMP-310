#ifndef CPU_H
#define CPU_H

#define DEFAULT_QUANTA 2

struct CPU { 
    // frame index
    int IP; 
    // offset within the frame
    int offset;
    // code line
    char IR[1000]; 
    // quanta
    int quanta;
} CPU;

int run(int);
void printCPU();

#endif //CPU_H