#ifndef INTERPRETERH
void interpreter(char* tokens[100], struct node * dataArr);
void runScript(char* file, struct node * arr);
void printCommands();
void invalidCommand();
#endif //INTERPRETERH