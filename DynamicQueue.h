#ifndef DYNAMIC_QUEUE_H
#define DYNAMIC_QUEUE_H

typedef struct node
{
    struct component data;
    struct node *next;
}node;

typedef struct dynamicQueue{
    struct node *front;
    struct node *end;
}dynamicQueue;


dynamicQueue* dqcreate();
void dqdisplay(dynamicQueue*);
void dqenqueue(struct component, dynamicQueue*);
int dqisempty(dynamicQueue*);
void dqdequeue(struct component*, dynamicQueue*);
void dqdestroy(dynamicQueue*);

#endif