#ifndef COMPONENT_H
#define COMPONENT_H

typedef struct component{
    int type;
    struct timespec startTime;
    long double waitTime;
    int id;
}component;

typedef struct endProduct{
    char id[13];
    long double endTime;
}endProduct;

void createComponent(int, component*);
void createEndProduct(component, component, component, endProduct*);

#endif