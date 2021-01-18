#ifndef _INCL_LINKED
#define _INCL_LINKED

#include "headers.h"

void addNodeToLikedlistEnd(struct LinkedList* list, struct Process processToAdd)
{
    struct Node * nodeToAdd = (struct Node *) malloc(sizeof(struct Node));  //create new node and assign the process to it.
    nodeToAdd->processInfo = processToAdd;
    nodeToAdd->next = NULL;

    if(list->head == NULL)
    {
        list->head = nodeToAdd;
        list->tail = nodeToAdd;
    }
    else
    {
        list->tail->next = nodeToAdd;
        nodeToAdd->previous = list->tail;
        list->tail = list->tail->next;
    }

    list->size ++;
}


void removeHeadNodeFromLikedlist(struct LinkedList* list)
{
    if(list->head == NULL)
    {
        return;
    }
    else if(list->head->next == NULL)
    {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        struct Node * tempNode = (struct Node *) malloc(sizeof(struct Node));  //create new node and assign the process to it.
        tempNode = list->head;
        list->head = list->head->next;
        free(tempNode);
    }
    list->size --;
}
#endif
