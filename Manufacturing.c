#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include "keys.h"
#include "Semaphores.h"
#include "Sem_shm_fun.h"
#include "Component.h"
#include "DynamicQueue.h"

#define PaintShopDelay 1000
#define InspectionDelay 1000
#define AssemblyDelay 1000

void Construction(int number, int type){

    int shmcp_id = shm_recover(SHMCP_KEY_FILE, sizeof(component));  //recover Construction-PaintShop shared memory segment
    component* sharedcp = (component*)shmat(shmcp_id, 0, 0);        //attach pointer to shm segment

    int semcp_id = sem_recover(SEMCP_KEY_FILE);                     //recover Construction-PaintShop semaphores

    component comp;
    
    for(int i = 0; i < number; i++){

        createComponent(type, &comp); 
        printf("A Type %d Component, IDNo: %d, was created.\n", comp.type, comp.id);

        if(!semaphore_p(semcp_id, 1))       //down(empty)
            exit(EXIT_FAILURE);

        if(!semaphore_p(semcp_id, 0))       //down(mutex)
            exit(EXIT_FAILURE);
   
        *sharedcp = comp;                   //move component to shared memory segment

        if(!semaphore_v(semcp_id, 0))       //up(mutex)
            exit(EXIT_FAILURE);

        if(!semaphore_v(semcp_id,2))        //up(full)
            exit(EXIT_FAILURE);
    }
    shmdt(sharedcp);                        //detach shared memory segment
}

void PaintShop(int number){

    int shmcp_id = shm_recover(SHMCP_KEY_FILE, sizeof(component));      //recover Construction-PaintShop shared memory segment
    component* sharedcp = (component*)shmat(shmcp_id, 0, 0);            //attach pointer to shm segment

    // recover shm segments for all types of components, attach pointers to segment

    int shmpitype1_id = shm_recover(SHMPITYPE1_KEY_FILE, sizeof(component));
    component* sharedpitype1 = (component*)shmat(shmpitype1_id, 0, 0);

    int shmpitype2_id = shm_recover(SHMPITYPE2_KEY_FILE, sizeof(component));
    component* sharedpitype2 = (component*)shmat(shmpitype2_id, 0, 0);

    int shmpitype3_id = shm_recover(SHMPITYPE3_KEY_FILE, sizeof(component));
    component* sharedpitype3 = (component*)shmat(shmpitype3_id, 0, 0);

    int semcp_id = sem_recover(SEMCP_KEY_FILE);                //recover Construction-PaintShop semaphores
    int sempitype1_id = sem_recover(SEMPITYPE1_KEY_FILE);      // recover PaintShop-Inspection semaphores for Type 1 Components
    int sempitype2_id = sem_recover(SEMPITYPE2_KEY_FILE);      // recover PaintShop-Inspection semaphores for Type 2 Components
    int sempitype3_id = sem_recover(SEMPITYPE3_KEY_FILE);      // recover PaintShop-Inspection semaphores for Type 3 Components

    component* sharedpi;
    int sempi_id;

    component comp;

    for(int i = 0; i < number; i++){

        if(!semaphore_p(semcp_id, 2))       //down(full)
            exit(EXIT_FAILURE);
        
        if(!semaphore_p(semcp_id, 0))       //down(mutex)
            exit(EXIT_FAILURE);

        comp = *sharedcp;

        if(!semaphore_v(semcp_id, 0))       //up(mutex)
            exit(EXIT_FAILURE);

        if(!semaphore_v(semcp_id,1))        //up(empty)
            exit(EXIT_FAILURE);

        // Calculate waiting time for this Component //

        struct timespec paintTime;
        clock_gettime(CLOCK_MONOTONIC, &paintTime);

        long double seconds = paintTime.tv_sec- comp.startTime.tv_sec;      //transpose to seconds
        long double ns = paintTime.tv_nsec - comp.startTime.tv_nsec;

        if(comp.startTime.tv_nsec > paintTime.tv_nsec){
            --seconds;
            ns += 1000000000;
        }
        comp.waitTime = seconds + ns/1000000000;

        ///////////

        usleep(comp.type * PaintShopDelay);      // PaintShop delay

        printf("A Type %d Component, IDNo: %d, was painted.\n", comp.type, comp.id); 

        // Move on to next step //

        switch(comp.type){                  //depending on the type of component, send to appropriate shm segment
            case 1:
                sharedpi = sharedpitype1;
                sempi_id = sempitype1_id;
                break;
            case 2:
                sharedpi = sharedpitype2;
                sempi_id = sempitype2_id;
                break;
            case 3:
                sharedpi = sharedpitype3;
                sempi_id = sempitype3_id;
                break;
            default:
                perror("INVALID COMPONENT TYPE");
                exit(EXIT_FAILURE);
        }

        if(!semaphore_p(sempi_id, 1))       //down(empty)
            exit(EXIT_FAILURE);

        if(!semaphore_p(sempi_id, 0))       //down(mutex)
            exit(EXIT_FAILURE);

        *sharedpi = comp;
        
        if(!semaphore_v(sempi_id, 0))       //up(mutex)
            exit(EXIT_FAILURE);

        if(!semaphore_v(sempi_id, 2))       //up(full)
            exit(EXIT_FAILURE);
    }
    shmdt(sharedcp);        //detach shm segments
    shmdt(sharedpitype1);
    shmdt(sharedpitype2);
    shmdt(sharedpitype3);
}

void Inspection(int number, int type){

    component comp;

    int shmpi_id;
    int sempi_id;
    component* sharedpi;

    switch(type){                           //depending on the type of component, recover appropriate shm segment ans semaphores set
        case 1:
            shmpi_id = shm_recover(SHMPITYPE1_KEY_FILE, sizeof(component));
            sempi_id = sem_recover(SEMPITYPE1_KEY_FILE);
            break;
        case 2:
            shmpi_id = shm_recover(SHMPITYPE2_KEY_FILE, sizeof(component));
            sempi_id = sem_recover(SEMPITYPE2_KEY_FILE);
            break;
        case 3:
            shmpi_id = shm_recover(SHMPITYPE3_KEY_FILE, sizeof(component));
            sempi_id = sem_recover(SEMPITYPE3_KEY_FILE);
            break;
        default:
            perror("INVALID COMPONENT TYPE");
            exit(EXIT_FAILURE);
            break;
    }
    
    sharedpi = (component*)shmat(shmpi_id, 0, 0);                       //attach pointer to shm

    int shmia_id = shm_recover(SHMIA_KEY_FILE, sizeof(component));      //recover Inspection-Assemply shm segment
    component* sharedia = (component*)shmat(shmia_id, 0, 0);            //attach pointer to shm
    
    int semia_id = sem_recover(SEMIA_KEY_FILE);                         //recover Inspection-Assembly semaphores

    for(int i = 0; i < number; i++){

        if(!semaphore_p(sempi_id, 2))       //down(full)
            exit(EXIT_FAILURE);
        
        if(!semaphore_p(sempi_id, 0))     //down(mutex)
            exit(EXIT_FAILURE);

        comp = *sharedpi;

        if(!semaphore_v(sempi_id, 0))     //up(mutex)
            exit(EXIT_FAILURE);

        if(!semaphore_v(sempi_id, 1))        //up(empty)
            exit(EXIT_FAILURE);

        usleep(comp.type * InspectionDelay);    //Inspection delay

        printf("A Type %d Component, IDNo: %d, was inspected.\n", comp.type, comp.id); 
        
        // Move on to next step//
        
        if(!semaphore_p(semia_id, 1))       //down(empty)
            exit(EXIT_FAILURE);

        if(!semaphore_p(semia_id, 0))       //down(mutex)
            exit(EXIT_FAILURE);
   
        *sharedia = comp;

        if(!semaphore_v(semia_id, 0))       //up(mutex)
            exit(EXIT_FAILURE);

        if(!semaphore_v(semia_id,2))        //up(full)
            exit(EXIT_FAILURE);
    }
    shmdt(sharedpi);        //detach shm
    shmdt(sharedia);        //detach shm
}

void Assembly(int number){

    component comp;

    int shmia_id = shm_recover(SHMIA_KEY_FILE, sizeof(component));      //recover Inspection-Assembly shm segment
    component* sharedia = (component*)shmat(shmia_id, 0, 0);            //attach pointer to shm

    int semia_id = sem_recover(SEMIA_KEY_FILE);                         //recover Inspection-Assembly semaphores set

    // Initialise Queues
    dynamicQueue* type1 = dqcreate();
    dynamicQueue* type2 = dqcreate();
    dynamicQueue* type3 = dqcreate();

    // Initialise Averages
    long double averageWaitTime = 0.0;
    long double averageProductTime = 0.0;

    for(int i = 0; i < number; i++){

        if(!semaphore_p(semia_id, 2))       //down(full)
            exit(EXIT_FAILURE);
        
        if(!semaphore_p(semia_id, 0))       //down(mutex)
            exit(EXIT_FAILURE);

        comp = *sharedia;

        if(!semaphore_v(semia_id, 0))       //up(mutex)
            exit(EXIT_FAILURE);
        
        if(!semaphore_v(semia_id,1))        //up(empty)
            exit(EXIT_FAILURE);

        averageWaitTime += comp.waitTime;   //add waiting time for this component to total waiting time

        switch(comp.type){                  //depending on the type of component, enqueue it to the appropriate queue
            case 1:
                dqenqueue(comp, type1);
                break;
            case 2:
                dqenqueue(comp, type2);
                break;
            case 3:
                dqenqueue(comp, type3);
                break;
            default:
                printf("INVALID COMPONENT TYPE\n");
                break;
        }
        if((!dqisempty(type1)) && (!dqisempty(type2)) && (!dqisempty(type3))){      //if there are items in all queues
            component one;
            dqdequeue(&one, type1);
            component two;
            dqdequeue(&two, type2);
            component three;
            dqdequeue(&three, type3);
            
            usleep(AssemblyDelay);     //Assembly delay

            // Create End Product
            endProduct final;
            createEndProduct(one, two, three, &final);
            printf("An End Product, IDNo: %s, was created.\n", final.id);
            averageProductTime += final.endTime;    //add production time to total production time
        }
    }
    shmdt(sharedia);        //detach shm segment

    averageWaitTime = averageWaitTime/number;               //calculate average waiting time
    averageProductTime = averageProductTime/(number/3);     //calculate average production time

    //destroy Queues
    dqdestroy(type1);
    dqdestroy(type2);
    dqdestroy(type3);

    //print results
    printf("\n\nThe average waiting time for the PaintShop is %Lf seconds.\n", averageWaitTime);
    printf("The average time for the fabrication of a Final Product is %Lf seconds.\n\n", averageProductTime);
}

