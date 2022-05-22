#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"shell.h"
#include"ram.h"
#include"cpu.h"
#include"interpreter.h"
#include"memorymanager.h"
#include"kernel.h"
#include"pcb.h"

/*
This is a node in the Ready Queue implemented as 
a linked list.
A node holds a PCB pointer and a pointer to the next node.
PCB: PCB
next: next node
*/
typedef struct ReadyQueueNode {
    PCB*  PCB;
    struct ReadyQueueNode* next;
} ReadyQueueNode;

ReadyQueueNode* head = NULL;
ReadyQueueNode* tail = NULL;
int sizeOfQueue = 0;


int main(int argc, char *argv[]) {
    int error=0;
    boot();
    error = kernel();
    return(error);
}

// initilizes everything
void boot(){

    // Initilize RAM
    for(int i=0; i<RAM_SIZE ; i++) ram[i] = NULL;

    // Initilizing the tracker used in the memeory manager
    initTracker();

    // Delete old and create new backing storage
    char* script = "rm -rf BackingStore/ > /dev/null 2>&1\nmkdir BackingStore > /dev/null 2>&1";
    /*
        Bash Script:
        rm -rf BackingStore/ > /dev/null 2>&1
        mkdir BackingStore > /dev/null 2>&1
    */
    system(script);
}


int kernel(){
    return shellUI();
}

/*
Adds a pcb to the tail of the linked list
*/
void addToReady(struct PCB* pcb) {
    ReadyQueueNode* newNode = (ReadyQueueNode *)malloc(sizeof(ReadyQueueNode));
    newNode->PCB = pcb;
    newNode->next = NULL;
    if (head == NULL){
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    sizeOfQueue++;
}

/*
Returns the size of the queue
*/
int size(){
    return sizeOfQueue;
}

/*
Pops the pcb at the head of the linked list.
pop will cause an error if linkedlist is empty.
Always check size of queue using size()
*/
struct PCB* pop(){
    PCB* topNode = head->PCB;
    ReadyQueueNode * temp = head;
    if (head == tail){
        head = NULL;
        tail = NULL;
    } else {
        head = head->next;
    }
    free(temp);
    sizeOfQueue--;
    return topNode;
}

/*
manages the order we run the PCBs in the CPU
also manages page faults by calling other functions
*/
int scheduler(){
    // set CPU quanta to default, IP to -1, IR = NULL
    CPU.quanta = DEFAULT_QUANTA;
    CPU.IP = -1;

    while (size() != 0){
        //REMOVE
        // printRam();
        // printTracker();

        //pop head of queue
        PCB* pcb = pop();

        // copy PC of PCB to IP of CPU
        // PC or IP point to the index of the current frame in ram
        CPU.IP = pcb->PC;
        // copy PC_offset of PCB to offset of CPU
        CPU.offset = pcb->PC_offset;

        int isOver = FALSE;
        int remaining;
        int quanta = DEFAULT_QUANTA;

        // if this is the final page in the program,
        // then check the number of lines code in the frame
        // if we are not in the last frame we will get a page intrupt
        // and the quanta doesn't matter.
        // note that PC_page start from 0 and pages_max is from 1 hence the +1
        if(pcb->PC_page+1 == pcb->pages_max){
            remaining = remaningInLastFrame(pcb);
            if (DEFAULT_QUANTA >= remaining) {
                isOver = TRUE;
                quanta = remaining;
            }
        }

        int errorCode = run(quanta);
        // the error code of 2 means we have had a page fault
        if(errorCode == 2){
            int pageError = pageIntrupt(pcb);
            if(pageError != 0) addToReady(pcb);
        }
        // the process is over
        else if (isOver){
            // REMOVE
            // printf("\n \nIs Over: \n \n ");
            // printf("Before Remove in kernel: \n");
            // printTracker();
            // printf("\n\n");

            removeFromRam(pcb);
            free(pcb);

            // REMOVE
            // printf("After Remove in kernel: \n");
            // printTracker();
            // printf("\n\n");
        }
        // the process is not over and the frame is not done
        else if (errorCode >= 0) {
            pcb->PC_offset += DEFAULT_QUANTA;
            addToReady(pcb);
        }
        // errorcode < 0, fetal error
        else{   // fatal error, shut the execution down and return
                // clean every thing up
            removeFromRam(pcb);
            return errorCode;
        }
    }
    return 0;
}

// function that manages page intrupts
int pageIntrupt(PCB* pcb){
    int nextPage = pcb->PC_page+1;
    // if the process if completed
    if(nextPage >= pcb->pages_max){
        removeFromRam(pcb);
        free(pcb);
        return 0;
    }
    else if(pcb->pageTable[nextPage] != -1){  // have next frame in ram
        pcb->PC_page = nextPage;
        pcb->PC = pcb->pageTable[nextPage];
        pcb->PC_offset = 0;
        
        return 1;
    }
    else{                  // don't have next frame in ram
        loadNewPages(pcb); // bring in the frame from the backing storage
        return 2;
    }
}

// loads one new pages into when we have a page fault and don't have a frame in ram
void loadNewPages(PCB* p){
    int nextPage = p->PC_page+1;
    p->PC_page = nextPage;
    int curFrame = findFrame();
    int victimFrame = -1;
    if(curFrame == -1) victimFrame = findVictim(p);
    FILE* f;

    // looking throught the innerTracker to find the file name of the PCB
    // so we can access the file in the BackingStore
    for(int i=0; i<3; i++){
        TrackerItem* item = innerTracker[i];

        if((item != NULL) && (item->pcb != NULL) && (item->pcb)->PC == p->PC){
            char path[100];
            sprintf(path, "./BackingStore/%d.txt", item->fileNum);
            f = fopen(path, "r");
            // REMOVE
            // printf("%s\n", path);
        }
    }

    if(curFrame == -1){
        loadPage(nextPage, f, victimFrame);
    }
    else{
        loadPage(nextPage, f, curFrame);
    }
    
    updatePageTable(p, nextPage, curFrame, victimFrame);
    p->PC = p->pageTable[nextPage];
    p->PC_offset = 0;
}

/*
Flushes every pcb off the ready queue in the case of a load error
and resets the innerTracker used in by the memory manager
*/
void emptyReadyQueue(){
    while (head!=NULL){
        ReadyQueueNode * temp = head;
        head = head->next;
        free(temp->PCB);
        free(temp);
    }
    for(int i=0; i<3; i++){
        if(innerTracker[i] != NULL) free(innerTracker[i]);
        innerTracker[i] = NULL;
    }
    sizeOfQueue =0;
}

/*
Clears the all the files in the backing store
*/
void clearBacking(){
    char* script = "rm -rf ./BackingStore/* > /dev/null 2>&1";
    /*
        Script:
        rm -rf ./BackingStore/* > /dev/null 2>&1
    */
    system(script);
}
