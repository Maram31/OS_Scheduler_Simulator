//Priority Ready Queue 
#include <stdio.h> 
#include <stdlib.h> 
#include "headers.h"

/*
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
  */
    
    
// Inserts a process to the most suited position in the list according to its priority
void insertWithPriority(struct LinkedList* ProcessesList, struct Process newProcess) {
        
    struct Node * newProcessNode;
    newProcessNode = (struct Node*)malloc(sizeof(struct Node));
    newProcessNode->processInfo = newProcess;
        
    if (ProcessesList->head == NULL) {                                                                       // linked list is empty so new process is inserted first in list
        ProcessesList->head = newProcessNode;
        newProcessNode ->previous = NULL;
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

        while (pointerToProcessNode->next != NULL) {                                                 // as long as the end of the linked list hasn't been reached 
                
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
void removeWithPriority(struct LinkedList * L) {
    if(L->head != NULL) {
        struct Node * N = L->head;       
        L->head = L->head->next;
        if(L->head != NULL) {
            L->head->previous = NULL;
        }
        free(N);
    }
}


void printLinkedList(struct LinkedList * L) {

    struct Node* N = L->head;
    
    if(N != NULL) {
        do {
            printf("%d ",N->processInfo.id);
            printf("%d ",N->processInfo.arrivalTime);
            printf("%d ",N->processInfo.runTime);
            printf("%d \n",N->processInfo.priority);
            //printf("%d ",N->processInfo.remainingTime);
            //printf("%d \n",N->processInfo.waitingTime);

            N = N->next;

        } while(N != NULL);
    } printf("\n\n");
}


// Check if list is empty
int isEmpty(struct LinkedList * L) {
    return (L->head) == NULL;
}





void insert_srtn(struct LinkedList* ProcessesList, struct Process newProcess) 
{
    struct Node * newProcessNode;
    newProcessNode = (struct Node*)malloc(sizeof(struct Node));
    newProcessNode->processInfo = newProcess;
    //newProcessNode->processInfo.isStarted =0;
    // if no nodes yet  
    if (ProcessesList->head == NULL) 
    {                                                                       // linked list is empty so new process is inserted first in list
        ProcessesList->head = newProcessNode;
        ProcessesList->size++;
        //printf("\nProcess with id %d is successfully inserted!", newProcess.id);
        return;
    }

    // if gonna insert at head
    if (newProcess.runTime < ProcessesList->head->processInfo.runTime) {                                   // new process has the highest priority and should be inserted first in linked list
        newProcessNode->next = ProcessesList->head;
        ProcessesList->head = newProcessNode;
        ProcessesList->size++;
        //printf("\nProcess with id %d is successfully inserted!", newProcess.id);
        return;
    } 

    if (newProcess.runTime > ProcessesList->head->processInfo.runTime)
    {                                                                                       // traverse the linked list to find best position to insert the new process in
        struct Node * ptr;                                                  // create node pointer to traverse the linked list
        ptr = ProcessesList->head;                                                      // initially pointing to beginning of linked list

        // if 1 node and insert not at head
        if(!ptr->next)
        {
            ProcessesList->head->next = newProcessNode;  
            ProcessesList->size++;
            //printf("\nProcess with id %d is successfully inserted!", newProcess.id);
            return;
        }
        int inserted = 0;
        while (ptr->next) 
        {                                                
            if (newProcess.runTime >= ptr->processInfo.runTime &&  newProcess.runTime <= ptr->next->processInfo.runTime ) 
            {  
                struct Node * temp = newProcessNode;            //create a temp node points to new process
                temp->next = ptr->next;                         // now make the new pcs point to next of the ptr
                ptr->next = temp;                               // then make the next of the ptr be the temp
                ProcessesList->size++;
                //printf("\nProcess with id %d is successfully inserted!", newProcess.id);
                return;
            }

            //reached el end, insert w 5alas at the end
            /*
            if(!ptr->next)
            {
                struct Node * ptrtemp = ptr;
                ptrtemp->next = newProcessNode;
                newProcessNode->next = NULL;
                ProcessesList->size++;
                printf("\nProcess with id %d is successfully inserted!", newProcess.id);
                return;
            }
            */
            ptr = ptr->next;
        }

        // didnt return therefore reached end of list
        struct Node * ptrtemp = ptr;
        ptrtemp->next = newProcessNode;
        newProcessNode->next = NULL;
        ProcessesList->size++;
        printf("\nProcess with id %d is successfully inserted!", newProcess.id);
        return;


    }
 }

//debugging
void presentid(struct LinkedList * L) {

    struct Node* N = L->head;
    printf("\nPresent ids are:"); 
    if(N != NULL) {
        do {
            printf("%d ",N->processInfo.id);
            N = N->next;
        } while(N != NULL);
    }
}


/* function to swap data of two nodes a and b*/
void swapnodes(struct Node *a, struct Node *b) 
{ 
    struct Process temp;
    temp = a->processInfo; 
    a->processInfo = b->processInfo; 
    b->processInfo = temp; 
} 



// Removes the process with the highest priority form the beginning of the list 

void remove_head(struct LinkedList * L) {
    if(L->head != NULL) {
        struct Node * N = L->head;       
        L->head = L->head->next;
        if(L->head != NULL) {
            L->head->previous = NULL;
        }
        //free(N);
    }
}


//Bubble sort algorithm for linked list
// Worst case cmplx is O(n^2), but the list will be almost sorted so will be O(n) 
void bubbleSort_srtn(struct LinkedList *L) 
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
            if (ptr1->processInfo.runTime > ptr1->next->processInfo.runTime) //If process closer to head bigger than its next, swap
            {  
                swapnodes(ptr1, ptr1->next); 
                swapped = 1; 
            } 
            ptr1 = ptr1->next; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
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
            if (ptr1->processInfo.runTime < ptr1->next->processInfo.runTime) //If process closer to head bigger than its next, swap
            {  
                swapnodes(ptr1, ptr1->next); 
                swapped = 1; 
            } 
            ptr1 = ptr1->next; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
} 
