#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
#include "shellmemory.h"


void addData(char* key, char* value ,struct node * dataArr){
	struct node* cur;
	//your variable curr should not be local and should be pointer to
	//a node since you want it pointing to the node in dataArr. 
	
	// traverse the array till you find either an empty slot or the key
	for(int i=0; i<1000; i++){

		//printf("Key: %s   Value: %s \n", key, value);
		cur = &dataArr[i];
		//printf("Before --> Key: %s    Value: %s at %d \n", cur->key, cur->value, i);

		// if key not there, put in the first empty slot
		if(cur->key == NULL){
			cur->key = strdup(key);
			cur->value = strdup(value);
			//printf("Pair added at: %d \n", i);
			//printf("After --> Key: %s    Value: %s at %d \n", cur->key, cur->value, i);
			return;
		}

		// if the key already exists
		else if(strcmp(cur->key,key) == 0){
			//printf("Value updated at: %d \n", i);
			cur->value = strdup(value);
			//printf("After --> Key: %s    Value: %s at %d \n", cur->key, cur->value, i);
			return;
		}
	}
}

void printValue(char* key, struct node * dataArr){
	struct node* cur;
	for(int i=0; i<1000; i++){
		cur = &dataArr[i];
		if(cur->key == NULL){
			printf("Variable does not exist\n");
			return;
		}
		else if(strcmp(cur->key,key) == 0){
			printf("%s = %s \n",cur->key, cur->value);
			return;
		}
	}
}