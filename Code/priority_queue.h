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

}

struct Node {
    struct Process processInfo;
    struct Node * next;
    struct Node * previous;
};

struct LinkedList {
	struct Node * head;
	struct Node * tail;
};
  
    
    
// Inserts a process to the most suited position in the list according to its priority
void insert(struct LinkedList* ProcessesList, struct Process newProcess) {
        
    struct Node * newProcessNode;
    newProcessNode = (struct Node*)malloc(sizeof(struct Node));
    newProcessNode->processInfo = newProcess;
        
    if (L->head == NULL) {                                                                       // linked list is empty so new process is inserted first in list
        L->head = newProcessNode;
        return;
    }

    if (newProcess.priority < L->head->processInfo.priority) {                                   // new process has the highest priority and should be inserted first in linked list
        newProcessNode->next = L->head;
        L->head = newProcessNode;
        return;
    } 

    else {                                                                                       // traverse the linked list to find best position to insert the new process in
        struct Node * pointerToProcessNode;                                                  // create node pointer to traverse the linked list

        pointerToProcessNode = L->head;                                                      // initially pointing to beginning of linked list

        while (firstProcess->next != NULL) {                                                 // as long as the end of the linked list hasn't been reached 
                
            if (newProcessNode->processInfo.priority >= N->next->processInfo.priority) {     // if position isn't found yet
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
