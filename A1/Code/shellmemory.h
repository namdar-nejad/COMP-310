#ifndef SHELLMEMORYH
struct node{
	char *key;
	char *value; 
};

void addData(char* key, char* value ,struct node * dataArr);
void printValue(char* key, struct node * dataArr);

#endif //SHELLMEMORYH