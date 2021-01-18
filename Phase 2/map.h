/*Author: Ahmed Osama
*   Needed by the buddy memory algorithm to allocate and free the memory
*   Similar to C++ map but with complexity o(n) instead of o(1)
*/
#include <stdlib.h>
#include <stdio.h>


struct map_element {
    int key;
	int value;
    struct map_element * next;
    struct map_element * previous;
    //struct map_element * previous;
};

struct map {
	struct map_element * head;
    //int size;
};

void remove_from_map(struct map * m, int key) {
    if(m->head == NULL)
        return;
    if(m->head->key == key)
    {
        m->head = m->head->next;
        return;
    }    
    struct map_element* prev = m->head;
    struct map_element* temp = m->head->next;

    while (temp != NULL)
    {
        if(temp->key == key)
        {
            prev->next = temp->next;
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    return;
     
}

int get_value_map(struct map * m, int key) {
    struct map_element * temp;
    temp = m->head;

    while (temp != NULL)
    {
        if(temp->key == key)
        {
            return temp->value;
        }
        temp = temp->next;
    }
    return -1;
    
    
}

void insert_to_map(struct map * m, int key, int value) {
    struct map_element * temp;
    temp = (struct map_element*)malloc(sizeof(struct map_element));
    
    struct map_element * prev;
    prev = (struct map_element*)malloc(sizeof(struct map_element));
    prev = NULL;    

    if (m->head == NULL)
    {
        temp->key = key;
        temp->value = value;
        temp->next = NULL;
        temp->previous = NULL;
        m->head = temp;
        return;
    }
    temp = m->head;

    while (temp != NULL)
    {
        if(temp->key == key)
        {
            temp->value = value;
            return;
        }
        prev = temp;
        temp = temp->next;
    }

    temp = (struct map_element*)malloc(sizeof(struct map_element));
    temp->key = key;
    temp->value = value;
    prev ->next = temp;
    temp->previous = prev;
    temp->next = NULL;

    return ;
    
    
}
