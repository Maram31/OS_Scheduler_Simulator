#include <stdlib.h>
#include <stdio.h>

struct vector_element {
    int first;
	int second;
    struct vector_element * next;
    struct vector_element * previous;
    //struct map_element * previous;
};

struct vector {
	struct vector_element * head;
    struct vector_element * tail;
    struct vector * next;
    struct vector * previous;
    int size;
    //int size;
};

void push_back(struct vector * m, int first, int second) {
    struct vector_element * temp;
    temp = (struct vector_element*)malloc(sizeof(struct vector_element));
    //printf("Pushing baack..\n");
    if (m->head == NULL)
    {
        temp->first = first;
        temp->second = second;
        temp->next = NULL;
        temp->previous = NULL;
        m->head = temp;
        m->tail = temp;
        m->size += 1;
        return;
    }

    temp->first = first;
    temp->second = second;
    temp->next = NULL;    
    temp->previous = m->tail;
    m->tail->next = temp;
    m->tail = temp;
    m->size += 1;
    return ;
        
}

int begin_first(struct vector * m) {
    if(m->head != NULL)
        return m->head->first;
    return -1;
}

int begin_second(struct vector * m) {
    if(m->head != NULL)
        return m->head->second;
    return -1;
}

int back_first(struct vector * m) {
    if(m->tail != NULL)
        return m->tail->first;
    return -1;
}

int back_second(struct vector * m) {
    if(m->tail != NULL)
        return m->tail->second;
    return -1;
}

void pop_back(struct vector * m) {
    if(m->tail == NULL)
        return;
    //printf("Pop back!\n");    
    if(m->size == 1)
    {
        m->head =NULL;
        m->tail = NULL;
        m->size = 0;
        return;
    }
    struct vector_element * prev = m->tail->previous;
    prev->next = NULL;
    m->tail = prev;
    m->size -= 1;
    return ;
        
}

void erase_begin(struct vector * m) {
    
    if(m->head == NULL)
        return;
    //printf("Erase begin\n");
    if(m->size == 1)
    {
        m->head =NULL;
        m->tail = NULL;
        m->size = 0;
        return;
    }
    m->head = m->head->next;
    m->size -= 1;
    return ;
        
}

void erase_at_index(struct vector * m, int idx) {
    if(idx == 0)
    {
        if(m->head != NULL)
        {
            m->head = m->head->next;
            m->size -= 1;
            return;
        }
        else
        {
            return;
        }
        
    }
    int k = 1;
    struct vector_element * temp = m->head->next;
    while(temp != NULL)
    {
        if(idx == k)
        {
            //printf("1.Erasing debug at idx %d\n", idx);
            struct vector_element * prev = temp->previous;
            //printf("2.Erasing debug at idx %d\n", idx);
            prev->next = temp->next;
            //printf("3.Erasing debug at idx %d\n", idx);
            
            if(temp->next == NULL)
            {
                m->tail = temp->previous;
            }
            else
            {
                temp -> next ->previous = prev;
            }
            

            m->size -= 1;
            return;
        }
        k += 1;
        temp = temp->next;
    }
    return ;
        
}

void print_vector(struct vector * test_vector)
{
    int counter = 0;
    struct vector_element * temp_debug = test_vector->head; 
    while(temp_debug != NULL)
    {
        printf("Pair: First = %d\tSecond = %d\tj: %d\n", temp_debug->first, temp_debug->second, counter);
        temp_debug = temp_debug->next;
        counter ++;
    }
}