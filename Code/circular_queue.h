
#include <stdio.h> 
#include <stdlib.h> 


struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int executionTime;
    int remainingTime;
    int waitingTime;

    bool isFinished;
    bool isReady;
    bool isRunning;

};

struct Node {
    struct processData processInfo;
    struct Node * next;
    struct Node * previous;
};

struct LinkedList {
	struct Node * head;
	struct Node * tail;
    int size;
};


void insertToQueue(struct LinkedList* ProcessesList, struct processData newProcess) {
        
    struct Node * newProcessNode;
    newProcessNode = (struct Node*)malloc(sizeof(struct Node));
    newProcessNode-> processInfo = newProcess;


    if (ProcessesList->head == NULL) {                                                                       // linked list is empty so new process is inserted first in list
        ProcessesList->head = newProcessNode;
        newProcessNode ->previous = NULL;
    }
    else {                 
        ProcessesList -> tail ->next = newProcessNode;       
        newProcessNode -> previous =  ProcessesList ->tail;        
    }

    ProcessesList -> tail = newProcessNode;
    ProcessesList ->head ->previous = newProcessNode;
    newProcessNode -> next = ProcessesList ->head;
    ProcessesList -> size += 1;
    return;
 };


void removeFromQueue(struct LinkedList * L, int id) {
    struct Node* currentNode = L->head; //Set the current node to the list head

    while (currentNode != NULL) //Loop as long as node is not null
    {
        if(currentNode -> processInfo.id == id)     //If found the required node to remove
        {
            if (L->size == 1)
            {
                L->head = NULL;
                L-> tail = NULL;
            }
            else
            {
                if(currentNode == L->head)
                {
                    L->head = L->head ->next;
                }
                
                currentNode ->previous ->next = currentNode->next;
                
            }
            
            free(currentNode); //Deallocate removed node
            return;
        }

        if (currentNode ->next != L->head)  //if not found check if the next node is not the head to prevent infinite looping
        {
            currentNode = currentNode ->next;
        }
        else
        {
            currentNode = NULL;
        }
    }
    
    return;

    
}
