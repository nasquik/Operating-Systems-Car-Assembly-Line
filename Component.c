#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include "Component.h"

void createComponent(int Type, component* a_component){

    clock_gettime(CLOCK_MONOTONIC, &a_component->startTime);    //calculate start time for this component
    a_component->type = Type;
    a_component->id = 1000+(rand()%9000);
};

void createEndProduct(component one , component two, component three, endProduct* endProd){

    sprintf(endProd->id, "%d%d%d", one.id, two.id, three.id);       //end Product ID is the used components' IDs combined
    
    // calculate production time for this product
    struct timespec endTime;
    clock_gettime(CLOCK_MONOTONIC, &endTime);

    // start time for this product is the earliest start time from the start times of the components used
    struct timespec min;

    if(one.startTime.tv_sec < two.startTime.tv_sec){
        min = one.startTime;
    }
    else if(one.startTime.tv_sec > two.startTime.tv_sec){
        min = two.startTime;
    }
    else{
        if(one.startTime.tv_nsec <= two.startTime.tv_nsec)
            min = one.startTime;
        else
            min = two.startTime;
    }

    if(min.tv_sec > three.startTime.tv_sec){
        min = three.startTime;
    }
    else if(min.tv_sec == three.startTime.tv_sec){
        if(min.tv_nsec > three.startTime.tv_nsec)
            min = three.startTime;
    }

    long double seconds = endTime.tv_sec - min.tv_sec;
    long double ns = endTime.tv_nsec - min.tv_nsec;

    if(min.tv_nsec > endTime.tv_nsec){
        --seconds;
        ns += 1000000000;
    }

    endProd->endTime = (double)seconds + (double)ns/(double)1000000000;
}