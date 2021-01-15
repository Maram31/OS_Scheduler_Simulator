#include <stdlib.h>
#include <stdio.h>
#include "vector.h"

struct vector_of_vectors {
	struct vector * head_vov;
    struct vector * tail_vov;
    int size;
    //int size;
};

void initialize_vov(struct vector_of_vectors * v, int cont_size) {
    struct vector * iterator ;
    struct vector* prev;

    struct vector *temp = (struct vector*)malloc(sizeof(struct vector));
    v->head_vov = temp;
    v->tail_vov = temp;
    iterator = v->head_vov;
    int i = 1;

    while (i <= cont_size)
    {
        temp = (struct vector*)malloc(sizeof(struct vector));
        iterator -> next = temp;
        temp->next = NULL;
        temp->previous = iterator;
        v->tail_vov = temp;
        iterator = temp;
        //prev = temp;
        //iterator = iterator->next;
        i++;
    }
    return;
}
