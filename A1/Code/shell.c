#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
#include "shellmemory.h"
#include "interpreter.h"
#include "shell.h"

// Main function
int main(int argc, char *argv[]) {
	initShell();
	struct node * dataArr= calloc(1000,sizeof(struct node));
	promptLoop(dataArr);
	return 0;
}

// Prompt loop
void promptLoop(struct node * dataArr){
		while(1){
		// vars
		char* tokens[100];
		char command[1000];

		// prompt
		printf("$ ");

		// function calls
		fgets(command, 999, stdin);
		parse(command, tokens);

		// int i=0;
		// while(tokens[i]!=NULL) { 
		// printf(">>%s<<\n",tokens[i]);
		// i++;
		// }

		interpreter(tokens, dataArr);
	}
}

// Parse function
void parse(char *command, char *tokens[]){
	char buffer[1000];
	int s=0, b=0, t=0, n=0;
	bool added;

	// while we have not reached the end of the command
	while(*(command+s) != '\0') {
		added = false;
		// skip leading spaces
		for(;*(command+s)==' ';s++);
		// skip tailing spaces
		//for(;*(command)==' ';n--);

		// exit if end of string
		if (*(command+s) == '\0') break;

		b=0;
		// otherwise, add word to buffer
		while(*(command+s)!=' ' && *(command+s)!='\0') {
			//printf("Curr char: %c \n", *(command+s));
			if(*(command+s) == ' ') s++, b++;
			else{
				added = true;
				buffer[b] = *(command+s);
				s++;
				b++;
			}
		}

		// create a token
		if (added == true){
			if (buffer[b-1] == '\n') buffer[b-1]='\0';
			else buffer[b]='\0';
			tokens[t] = strdup(buffer);
			t++;
			//printf("Token %d is %s \n",t,buffer);
		}
	}
	tokens[t] = NULL;
}

void initShell(){
	printf("Welcome to the Nami shell!\nVersion 1.0 Created January 2020\n");
}