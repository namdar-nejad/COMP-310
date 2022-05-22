#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include"shellmemory.h"
#include"shell.h"
#include"kernel.h"
#include"ram.h"
#include"memorymanager.h"
#include"DISK_driver.h"
#include"DISK_driver_problem1.h"
#include"DISK_driver_problem2.h"
#include"DISK_driver_problem3.h"

#define TRUE 1
#define FALSE 0

int checkInput(char* words[]);
char* parseData(char* words[]);
int set(char * words[]);
int interpreter(char* words[]);

//**** FINAL PROJECT START ****//

/*
    open_EXAM <file_ID_in_active_file_table> filename
*/
int openCommandEXAM(char *parsedCommand[]){
    // new openFile function in DISK_driver_problem1.c
    int errorCode = open_EXAM_function(atoi(parsedCommand[1]), parsedCommand[2]);

    // check errorCode
    // if(index < 0) return -11;

    // the error massage in printed in the DISK_driver_problem1.c
    // since we were told to handle everything inside the files given
    // return to the prompt no matter what
    return 0;
}

/*
    write_EXAM <file_ID_in_active_file_table> [alphanumeric strings separated by spaces]
*/
int writeCommandEXAM(char *parsedCommand[]){

    // the index passed to the function
    int activeIndex = atoi(parsedCommand[1]);

    // check if we have a file in that index of the active file table
    if(active_file_table[activeIndex] == NULL){
        printf("ERROR: Open the file first.\n");
        return 0;
    }

    // check data input syntax
    if(checkInput(parsedCommand) == -1) {
        printf("invalid data input for Write\n");
        return 0;
    }

    // parse the data input
    char * data = parseData(parsedCommand);

    // find fat index for the file to write to
    int fatIndex = -1;
    for(int i=0; i<FAT_SIZE; i++){
        if(fat[i].ftIndex == activeIndex){
            fatIndex = i;
            break;
        }
    }

    // check if we have found the FAT index
    if(fatIndex == -1) return -10;

    // call the old writeBlock to write to file and check for errors
    if(writeBlock(fatIndex, data) != 1) return -10;

    // free data buffer
    free(data);

    return 0;
}

// read_EXAM <file_ID_in_active_file_table> var
int readCommandEXAM(char *parsedCommand[]){

    // the index passed to the function
    int activeIndex = atoi(parsedCommand[1]);

    // check if we have a file in that index of the active file table
    if(active_file_table[activeIndex] == NULL){
        printf("ERROR: Open the file first.\n");
        return 0;
    }

    // find fat index for the file to read from
    int fatIndex = -1;
    for(int i=0; i<FAT_SIZE; i++){
        if(fat[i].ftIndex == activeIndex){
            fatIndex = i;
            break;
        }
    }

    // check if we have found the FAT index
    if(fatIndex == -1) return -10;

    // read blocks of the opened file, and check for error
    if(readBlock(fatIndex) != 1) return -9;

    // set varible to the block_buffer value
    char * setVarInput[] = {"set", parsedCommand[2], getBlockBuffer()};
    set(setVarInput);
    
    return 0;
}

// close_EXAM <file_ID_in_active_file_table>
int closeCommandEXAM(char *parsedCommand[]){
    close_EXAM_function(atoi(parsedCommand[1]));
    return 0;
}

// seek_EXAM <file_ID_in_active_file_table> <offset>
int seekCommandEXAM(char *parsedCommand[]){
    seek_EXAM_function(atoi(parsedCommand[1]), atoi(parsedCommand[2]));
    return 0;    
}

int readpipeCommandEXAM(char *parsedCommand[]){
    // the index passed to the function
    int activeIndex = atoi(parsedCommand[1]);

    // check if we have a file in that index of the active file table
    if(active_file_table[activeIndex] == NULL){
        printf("ERROR: Open the file first.\n");
        return 0;
    }

    // find fat index for the file to read from
    int fatIndex = -1;
    for(int i=0; i<FAT_SIZE; i++){
        if(fat[i].ftIndex == activeIndex){
            fatIndex = i;
            break;
        }
    }

    // check if we have found the FAT index
    if(fatIndex == -1) return -10;

    // read the file and copy the data to the approriate buffer
    if(readBlock(fatIndex) != 1) return -9;

    // copy the values read to the buffer
    strcpy(pipeArr[fatIndex], getBlockBuffer());
    
    // set varible to the block_buffer value bu passing the command to the set command
    char * setVarInput[] = {"set", parsedCommand[4], strdup(pipeArr[fatIndex])};
    return set(setVarInput);
}

//**** FINAL PROJECT END ****//


/*
This function takes an array of string. 
First string is the "set" command
Second string is a variable name
Third string is a value
It assigns that value to a environment variable with that variable name in the shell memory array.
Return ERRORCODE -1 if out of memory else 0
*/
int set(char * words[]){
    char* varName = words[1];
    char* value = words[2];
    int errorCode = setVariable(varName,value);
    return errorCode;
}

int mount(char * words[]){
    char* name = words[1];
    char* number_of_blocks = words[2];
    char* block_size = words[3];

    char partitionPath[100];
    sprintf(partitionPath, "./PARTITION/%s", name);
    int errorCode;

    saveOldPartition();
    // a partiton exists
    if(access(partitionPath, F_OK ) == 0) {
        // mount
        errorCode = mountFS(name);
        if(errorCode == 0)  return -8;
    }
    else {
        // create partition
        errorCode = partitionMethod(name,atoi(block_size),atoi(number_of_blocks));
        if(errorCode == 0) return -7;
        // mount
        // errorCode = mountFS(name);
        if(errorCode == 0)  return -8;
    }

    return 0 ;
}

int Write(char* words[]){
    int errorCode = 0;
    char * filename = words[1];

    if(checkInput(words) == -1) {
        printf("invalid data input for Write\n");
        return errorCode;
    }

    char * data = parseData(words);
    int index = openfile(filename);
    if(index == -1) return -11;
    errorCode = writeBlock(index, data);
    
    free(data);
    if(errorCode != 1) return -10;

    return 0;
}

int Read(char * words[]){
    int errorCode = 0;
    char * filename = words[1];
    char * var = words[2];

    int fatIndex = openfile(filename);

    // read blocks of the opened file
    errorCode = readBlock(fatIndex);
    if(errorCode != 1) return -9;

    // set varible to the block_buffer value
    char * setVarInput[] = {"set", var, getBlockBuffer()};
    errorCode = set(setVarInput);
    
    return errorCode;
}

/*
parses the data input to write
*/
char* parseData(char* words[]){
    char input[1000];
    memset(input, '\0', sizeof(input));

    int z=0;
    for(int i=2; i<10; i++){
        if(strcmp(words[i],"_NONE_")==0) break;

        // add spaces between tokens
        if(i != 2){
            input[z] = ' ';
            z++;
        }

        for(int j=0; ;j++){
            if((words[i][j]) == '\0') break;
            input[z] = (words[i][j]);
            z++;
        }
    }
    
    char rtn[strlen(input)+1];

    for(int i=0, j=0; i<strlen(input)+1;i++){
        if((input[i] != '[') && (input[i] != ']')){
            rtn[j] = input[i];
            j++;
        }
    }

    return (strdup(rtn));
}

/*
checks the data input format to write
*/
int checkInput(char* words[]){
    if(words[2][0] != '[') return -1;
    for(int i=2; i<10; i++){
        if(strcmp(words[i+1],"_NONE_")==0){
            int len = strlen(words[i]);
            if(words[i][len-1] == ']') return 0;
            else return -1;
        }
    }
    return 0;
}

/*
This function takes an array of string. 
First string is the "print command".
Second string is the variable name.
It will print the value associated with that variable if it exists.
Else it will print an appropriate message.
Return 0 if successful.
*/
int print(char * words[]){

    char* varName = words[1];
    char* value = getValue(varName);

    if (strcmp(value,"_NONE_")==0) {
        // If no variable with such name exists, display this message
        printf ("Variable does not exist\n");
    } else {
        // else display the variable's value
        printf("%s\n",value);
    }
    return 0;
}

/*
This function takes an array of string.
First string is the "run" command
Second string is script filename to execute
Returns errorCode
*/
static int run(char * words[]){

    char * filename = words[1];
    FILE * fp = fopen(filename,"r");
    int errorCode = 0;
    // if file cannot be opened, return ERRORCODE -3
    if (fp==NULL) return -3;
    char buffer[1000];
    printf("/////////////////////////////// STARTING EXECUTION OF %s ///////////////////////////////\n",filename);
    while (!feof(fp)){
        fgets(buffer,999,fp);
        errorCode = parse(buffer);
        // User input the "quit" command. Terminate execution of this script file.
        if (errorCode == 1) {
            // Run command successfully executed so ERRORCODE 0. Stop reading file.
            errorCode = 0;
            break;
        } else if (errorCode != 0) {
            // An error occurred. Display it and stop reading the file.
            //removing the extra carriage return
            buffer[strlen(buffer)-2]='\0';
            displayCode(errorCode,buffer);
            break;
        }
    }
    printf("/////////////////////////////// Terminating execution of %s ///////////////////////////////\n",filename);
    fclose(fp);
    return 0;
}

int exec(char * words[]){
    char * filename[3] = { "_NONE_", "_NONE_", "_NONE_"};
    int errorCode = 0;
    for (int i = 0; i < 3; i++)
    {
        if ( strcmp(words[i+1],"_NONE_") != 0 ) {
            filename[i] = strdup(words[i+1]);
            FILE * fp = fopen(filename[i],"r");
            if (fp == NULL) {
                errorCode = -3;
            } else {
                int err = launcher(fp);
                // if launcher failed, set errorCode to -6 for LAUNCHING ERROR
                if ( err == 0){
                    errorCode = -6;
                }
            }
            if ( errorCode < 0){
                displayCode(errorCode,words[i]);
                printf("EXEC COMMAND ABORTED...\n");
                emptyReadyQueue();
                clearRAM();
                return 0;
            }
        }
    }

    printf("|----------| ");
    printf("\tSTARTING CONCURRENT PROGRAMS ( ");
    for (int i = 0; i < 3; i++)
    {
        if ( strcmp(filename[i],"_NONE_") != 0 ){
            printf("%s ", filename[i]);
        }
    }
    printf(")\t|----------|\n");

    scheduler();

    printf("|----------| ");
    printf("\tTERMINATING ALL CONCURRENT PROGRAMS");
    printf("\t|----------|\n");
    return 0;
}

/*
This functions takes a parsed version of the user input.
It will interpret the valid commands or return a bad error code if the command failed for some reason
Returns:
ERRORCODE  0 : No error and user wishes to continue
ERRORCODE  1 : Users wishes to quit the shell / terminate script
ERRORCODE -1 : RAN OUT OF SHELL MEMORY
ERRORCODE -2 : INCORRECT NUMBER OF ARGUMENTS
ERRORCODE -3 : FILE DOES NOT EXIST
ERRORCODE -4 : UNKNOWN COMMAND. TRY "help"
*/
int interpreter(char* words[]){
    //default errorCode if no error occurred AND user did not enter the "quit" command
    int errorCode = 0;
    //At this point, we are checking for each possible commands entered
    if ( strcmp(words[0],"help") == 0 ) {
        
        // if it's the "help" command, we display the description of every commands
        printf("-------------------------------------------------------------------------------------------------------\n");
        printf("COMMANDS\t\t\tDESCRIPTIONS\n");
        printf("-------------------------------------------------------------------------------------------------------\n");
        printf("help\t\t\t\tDisplays all commands\n");
        printf("quit\t\t\t\tTerminates the shell\n");
        printf("set VAR STRING\t\t\tAssigns the value STRING to the shell memory variable VAR\n");
        printf("print VAR\t\t\tDisplays the STRING value assigned to the shell memory variable VAR\n");
        printf("run SCRIPT.TXT\t\t\tExecutes the file SCRIPT.txt\n");
        printf("exec p1 p2 p3\t\t\tExecutes programs p1 p2 p3 concurrently\n");
        printf("-------------------------------------------------------------------------------------------------------\n");

    } else if ( strcmp(words[0],"quit") == 0) {

        // if it's the "quit" command
        //errorCode is 1 when user voluntarily wants to quit the program.
        errorCode = 1;

    } else if ( strcmp(words[0],"set") == 0 ) {
        // if it's the "set VAR STRING" command
        // check for the presence or 2 more arguments
        // If one argument missing, return ERRORCODE -2 for invalid number of arguments
        if ( ( strcmp(words[1],"_NONE_") == 0 ) || ( strcmp(words[2],"_NONE_") == 0 ) ) {
            errorCode = -2;
        } else {
            // ERRORCODE -1 : Out of Memory might occur
            errorCode = set(words);
        }
    }  else if ( strcmp(words[0],"print") == 0 ) {
        // if it's the "print VAR" command
        // if there's no second argument, return ERRORCODE -2 for invalid number of arguments
        if ( strcmp(words[1],"_NONE_") == 0 ) return -2;

        // Call the print function
        errorCode = print(words);

    } else if ( strcmp(words[0],"run") == 0 ) {
        // if it's the "run SCRIPT.TXT" command
        // check if there's a second argument, return ERRORCODE -2 for invalid number of arguments
        if ( strcmp(words[1],"_NONE_") == 0 ) return -2;

        //Error will be handled in the run function. We can assume that after the run 
        //function terminate, the errorCode is 0.
        errorCode = run(words);
    } else if ( strcmp(words[0],"exec") == 0 ) {
        // if it's the "exec" command
        // check if there's at least 2 arguments and not >= 4 arguments
        if ( strcmp(words[1],"_NONE_") == 0  || strcmp(words[4],"_NONE_") != 0 ) return -2;

        errorCode = exec(words);
    } 
    // strcmp(words[0],"exec") == 0 
    else if (strcmp(words[0],"mount") == 0){
        // all three arguments are required
        if ( strcmp(words[1],"_NONE_") == 0 
            || strcmp(words[2],"_NONE_") == 0 
            || strcmp(words[3],"_NONE_") == 0 
            || strcmp(words[4],"_NONE_") != 0 ) return -2;

        errorCode = mount(words);
    }
    else if (strcmp(words[0],"write") == 0){

        if ( strcmp(words[1],"_NONE_") == 0) return -2;

        errorCode = Write(words);
    }
    else if(strcmp(words[0],"read") == 0){

        if (strcmp(words[1],"_NONE_") == 0 
            || strcmp(words[2],"_NONE_") == 0 
            || strcmp(words[3],"_NONE_") != 0 ) return -2;

        errorCode = Read(words);
    }
    else if(strcmp(words[0],"open_EXAM") == 0){

        // check if the correct number of parameters have been passed
        if (strcmp(words[1],"_NONE_") == 0 
            || strcmp(words[2],"_NONE_") == 0 
            || strcmp(words[3],"_NONE_") != 0 ) return -2;

        errorCode = openCommandEXAM(words);
    }
    else if(strcmp(words[0],"read_EXAM") == 0){

        // check if the correct number of parameters have been passed
        if (strcmp(words[1],"_NONE_") == 0 
            || strcmp(words[2],"_NONE_") == 0 
            || strcmp(words[3],"_NONE_") != 0 ) return -2;

        errorCode = readCommandEXAM(words);
    }
    else if(strcmp(words[0],"write_EXAM") == 0){

        // check if the correct number of parameters have been passed
        if (strcmp(words[1],"_NONE_") == 0 
            || strcmp(words[2],"_NONE_") == 0) return -2;

        errorCode = writeCommandEXAM(words);
    }
    else if(strcmp(words[0],"close_EXAM") == 0){

        // check if the correct number of parameters have been passed
        if (strcmp(words[1],"_NONE_") == 0 
            || strcmp(words[2],"_NONE_") != 0) return -2;

        errorCode = closeCommandEXAM(words);
    }
    else if(strcmp(words[0],"seek_EXAM") == 0){

        // check if the correct number of parameters have been passed
        if (strcmp(words[1],"_NONE_") == 0 
            || strcmp(words[2],"_NONE_") == 0 
            || strcmp(words[3],"_NONE_") != 0 ) return -2;

        errorCode = seekCommandEXAM(words);
    }

    // NEW:
    else if(strcmp(words[0],"readpipe_EXAM") == 0){

        int errorCode = -2;

        // check if the correct number of parameters have been passed in the gereral case
        if (strcmp(words[1],"_NONE_") == 0 
            || strcmp(words[2],"_NONE_") == 0 
            || strcmp(words[5],"_NONE_") != 0 ) return -2;

        // check if the command is a pipeline command
        else if (strcmp(words[2],"|") == 0){

            // check the parameters passed to the pipeline command
            if(strcmp(words[3],"_NONE_") == 0 
            || strcmp(words[4],"_NONE_") == 0) return -2;

            errorCode = readpipeCommandEXAM(words);
        }

        // check the parameters passed to the normal command
        else if (strcmp(words[3],"_NONE_") == 0){
            // for the normal command we just call readCommandEXAM()
            errorCode = readCommandEXAM(words);
        }

        return errorCode;
    }
    // END;

    else {
        // Error code for unknown command
        errorCode = -4;
    }

    return errorCode;
}