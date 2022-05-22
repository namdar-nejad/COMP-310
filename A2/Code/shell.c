#include "interpreter.h"
#include "shellmemory.h"
#include "shell.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


void shellUI(){
    printf("Welcome to the Namdar shell!\n"
           "Version 1.0 Created January 2020\n");

    shell_memory_initialize();
    prompt();
    shell_memory_destory();
}

void prompt(){
    while (!feof(stdin)){
        printf("$ ");
        fflush(stdout);

        char *line = NULL;
        size_t linecap = 0;
        if (getline(&line, &linecap, stdin) == -1){
            break;
        }

        (void)interpret(line);
        free(line);
    }
}