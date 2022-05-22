#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
#include "shellmemory.h"
#include "interpreter.h"
#include "shell.h"

void interpreter(char* tokens[100], struct node * dataArr){
	char* first = tokens[0];
	char* last;
	//strlen("Just added this as a libc function with no SysCall");
	if (strcmp(first, "") == 0){
		return;
	}
	else if (strcmp(first, "help") == 0){
		last = tokens[1];
		if(last == NULL || *last == '\0' || *last == ' '){
			printCommands();
			return;
		}
			invalidCommand(dataArr, "usage: help");
	}
	else if (strcmp(first, "quit") == 0){
		last = tokens[1];
		if(last == NULL || *last == '\0' || *last == ' '){
			printf("Bye! \n");
			exit(0);
		}
			invalidCommand(dataArr, "usage: quit");
	}
	else if (strcmp(first, "set") == 0){
		char* key = tokens[1];
		char* value = tokens[2];
		last = tokens[3];
		if(last == NULL || *last == '\0' || *last == ' '){
			if(key != NULL && *key != '\0' && *key != ' '){
				if(value != NULL && *value != '\0' && *value != ' '){
					addData(key, value, dataArr);
					return;
				}
			}
		}
		invalidCommand(dataArr, "usage: set <varName> <varValue>");
	}
	else if (strcmp(first, "print") == 0){
		char* key = tokens[1];
		last = tokens[2];
		if(last == NULL || *last == '\0' || *last == ' '){
			if(key != NULL && *key != '\0' && *key != ' '){
				printValue(key, dataArr);
				return;
			}
		}
			invalidCommand(dataArr, "usage: print <varName>");
	}
	else if (strcmp(first, "run") == 0){
		char* path = tokens[1];
		last = tokens[2];
		if(last == NULL || *last == '\0' || *last == ' '){
			if(path != NULL && *path != '\0' && *path != ' '){
				runScript(path, dataArr);
				return;
			}
		}
			invalidCommand(dataArr, "usage: run <scriptName.ext>");
	}
	else invalidCommand(dataArr, "command doesn't exist");
}

void runScript(char* file, struct node * arr){
	FILE *fp;
	char buff[1000];
	char* tokens[100];
	fp = fopen(file, "r");
	if(fp == NULL) printf("Script not found\n");
	else{
		while( fgets (buff, 1000, fp) != NULL ) { 
			//printf("line: %s \n", buff);
			parse(buff, tokens);
			interpreter(tokens, arr);
		}
		fclose(fp);
	}
}

void printCommands(){
	printf("\nHelp Page:\n");
	printf("help: Displays all the commands\n");
	printf("quit: Exits / terminates the shell\n");
	printf("set VAR STRING: Assigns a value to shell memory\n");
	printf("print VAR: Displays the STRING assigned to VAR\n");
	printf("run SCRIPT.TXT: Executes the file SCRIPT.TXT\n");
	printf("\n");
}

void invalidCommand(struct node * dataArr, char * errorSec){
	printf("> Inavild Command; ' %s '\n", errorSec);
	promptLoop(dataArr);
}