#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Component.h"
#include "DynamicQueue.h"

dynamicQueue* dqcreate(){
    /*Function for the creation and initilisation
    of a dynamic queue*/

    struct dynamicQueue* q = malloc(sizeof(struct dynamicQueue));
    q->front = NULL;
    q->end = NULL;
    return q;
};

void dqdisplay(dynamicQueue* q){
    /*Function for the display of all the items in
    the dynamic queue. Not present in program, mainly
    used for testing*/

    struct node* i = q->front;
    while(i != NULL){
        printf("%d\t", i->data.type);
        i = i->next;
    }
    printf("\n");
};

int dqisempty(dynamicQueue* q){
    /*Function to determine if dynamic queue
    is empty*/
    return (q->front == NULL);
}

void dqenqueue(struct component comp, dynamicQueue* q){
    /*Function to enqueue a component to the
    dynamic queue*/

    node* i = malloc(sizeof(struct node));
    i->data = comp;
    i->next = NULL;
    if(q->end == NULL){
        q->front = i;
        q->end = i;
    }
    else{
        q->end->next = i;
        q->end = q->end->next;
    }
};

void dqdequeue(struct component* comp, dynamicQueue* q){
    /*Function to dequeue a component from
    the dynamic queue and copy its contents
    to the pointer given*/

    if(q->front == NULL){
        printf("The Queue is Empty!\n");
    }
    else{
        node* i;
        i = q->front;
        q->front = q->front->next;

        if(q->front == NULL){
            q->end = NULL;
        }
        *comp = i->data; 
        free(i);
    }
};

void dqdestroy(dynamicQueue* q){
    /*Function for the destruction
    of a dynamic queue*/

    struct component temp;

    while(q->front != NULL){
        dqdequeue(&temp, q);
    }
    free(q);
};