//Priority Ready Queue 
#include <stdio.h> 
#include <stdlib.h> 

//Process control block for a process
struct Process {
	int id;
	int arrivalTime;
	int priority;
	bool isRunning;
	int executionTime;
    int remainingTime;
    int waitingTime;
    bool isFinished;
    bool isReady;
};

struct Node {
    struct Process processInfo;
    struct Node * next;
    struct Node * previous;
};

struct LinkedList {
	struct Node * head;
	struct Node * tail;
    int size;
};
  
    
    
// Inserts a process to the most suited position in the list according to its priority
void insert(struct LinkedList* ProcessesList, struct Process newProcess) {
        
    struct Node * newProcessNode;
    newProcessNode = (struct Node*)malloc(sizeof(struct Node));
    newProcessNode->processInfo = newProcess;
        
    if (ProcessesList->head == NULL) {                                                                       // linked list is empty so new process is inserted first in list
        ProcessesList->head = newProcessNode;
        return;
    }

    if (newProcess.priority < ProcessesList->head->processInfo.priority) {                                   // new process has the highest priority and should be inserted first in linked list
        newProcessNode->next = ProcessesList->head;
        ProcessesList->head = newProcessNode;
        return;
    } 

    else {                                                                                       // traverse the linked list to find best position to insert the new process in
        struct Node * pointerToProcessNode;                                                  // create node pointer to traverse the linked list

        pointerToProcessNode = ProcessesList->head;                                                      // initially pointing to beginning of linked list

        while (ProcessesList->head->next != NULL) {                                                 // as long as the end of the linked list hasn't been reached 
                
            if (newProcessNode->processInfo.priority >= pointerToProcessNode->next->processInfo.priority) {     // if position isn't found yet
                pointerToProcessNode = pointerToProcessNode->next;                           // move node pointer to the next node
            }
                
            else {                                                                           // position is found
                break;                                                        
            }

        }

        newProcessNode->next = pointerToProcessNode->next;                                   // insert new process either at the end of the list or at the chosen position
        pointerToProcessNode->next= newProcessNode;
    }
 }




// Removes the process with the highest priority form the beginning of the list 

void remove(struct LinkedList * L) {
    if(L->head != NULL) {
        struct Node * N = L->head;       
        L->head = L->head->next;
        if(L->head != NULL) {
            L->head->previous = NULL;
        }
        free(N);
    }
}


//Bubble sort algorithm for linked list
// Worst case cmplx is O(n^2), but the list will be almost sorted so will be O(n) 
void bubbleSort_asc(struct LinkedList *L) 
{ 
    int swapped, i; 
    struct Node *ptr1; 
    struct Node *lptr = NULL; 
  
    /* Checking for empty list */
    if (L->head == NULL) 
        return; 
  
    do
    { 
        swapped = 0; 
        ptr1 = L->head; 
  
        while (ptr1->next != lptr) 
        { 
            /*HERE EXECUTION TIME IS USED IN CASE OF SRTN, CAN BE SWAPPED FOR PRIORITY*/
            if (ptr1->processInfo.executionTime > ptr1->next->processInfo.executionTime) //If process closer to head bigger than its next, swap
            {  
                swap(ptr1, ptr1->next); 
                swapped = 1; 
            } 
            ptr1 = ptr1->next; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
} 
  
/* function to swap data of two nodes a and b*/
void swap(struct Node *a, struct Node *b) 
{ 
    struct Process temp;
    temp = a->processInfo; 
    a->processInfo = b->processInfo; 
    b->processInfo = temp; 
} 

//Bubble sort descending for HPF
void bubbleSort_desc(struct LinkedList *L) 
{ 
    int swapped, i; 
    struct Node *ptr1; 
    struct Node *lptr = NULL; 
  
    /* Checking for empty list */
    if (L->head == NULL) 
        return; 
  
    do
    { 
        swapped = 0; 
        ptr1 = L->head; 
  
        while (ptr1->next != lptr) 
        { 
            /*HERE EXECUTION TIME IS USED IN CASE OF SRTN, CAN BE SWAPPED FOR PRIORITY*/
            if (ptr1->processInfo.executionTime < ptr1->next->processInfo.executionTime) //If process closer to head bigger than its next, swap
            {  
                swap(ptr1, ptr1->next); 
                swapped = 1; 
            } 
            ptr1 = ptr1->next; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
} 

