#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h> 
#include "keys.h"
#include "Semaphores.h"
#include "Sem_shm_fun.h"
#include "Component.h"
#include "Manufacturing.h"
#include "DynamicQueue.h"

int main(int argc, char* argv[]){

    int componentsNumber = 0;

    if (argc != 2){                      //User must provide number of components to be created at each stage
        printf("ERROR! Wrong input!\n");
        return -1;
    }
    else{
        componentsNumber = atoi(argv[1]);
        printf("The number of components created at each Construction stage is %d.\n", componentsNumber);
    }

                            /* INITIALISATION */

    ////// Initialise SHM segments //////

    int shmcp_id = shm_create(SHMCP_KEY_FILE, sizeof(component), 132);
    int shmpitype1_id = shm_create(SHMPITYPE1_KEY_FILE, sizeof(component), 276);
    int shmpitype2_id = shm_create(SHMPITYPE2_KEY_FILE, sizeof(component), 497);
    int shmpitype3_id = shm_create(SHMPITYPE3_KEY_FILE, sizeof(component), 571);
    int shmia_id = shm_create(SHMIA_KEY_FILE,sizeof(component), 377);
    
    ////// Initialise Construction-PaintShop Semaphores Set //////

    int semcp_id = sem_create(SEMCP_KEY_FILE, 3, 480);
    
    if(!set_semvalue(semcp_id, 0, 1)){                      //mutex = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    if(!set_semvalue(semcp_id, 1, 1)){                      //empty = 1
        perror("Could not set value of second semaphore");
        exit(4); 
    }

    if(!set_semvalue(semcp_id, 2, 0)){                      //full = 0
        perror("Could not set value of second semaphore");
        exit(4); 
    }

    ////// Initialise PaintShop-Inspection Semaphore Set for Type 1 Components //////

    int sempitype1_id = sem_create(SEMPITYPE1_KEY_FILE, 3, 698);
    
    if(!set_semvalue(sempitype1_id, 0, 1)){                   //mutex = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    if(!set_semvalue(sempitype1_id, 1, 1)){                   //empty = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    if(!set_semvalue(sempitype1_id, 2, 0)){                   //full = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    ////// Initialise PaintShop-Inspection Semaphore Set for Type 2 Components //////

    int sempitype2_id = sem_create(SEMPITYPE2_KEY_FILE, 3, 700);
    
    if(!set_semvalue(sempitype2_id, 0, 1)){                   //mutex = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    if(!set_semvalue(sempitype2_id, 1, 1)){                   //empty = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    if(!set_semvalue(sempitype2_id, 2, 0)){                   //full = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    ////// Initialise PaintShop-Inspection Semaphore Set for Type 3 Components //////

    int sempitype3_id = sem_create(SEMPITYPE3_KEY_FILE, 3, 334);
    
    if(!set_semvalue(sempitype3_id, 0, 1)){                   //mutex = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    if(!set_semvalue(sempitype3_id, 1, 1)){                   //empty = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    if(!set_semvalue(sempitype3_id, 2, 0)){                   //full = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    ////// Initialise Inspection-Assembly Semaphores Set //////

    int semia_id = sem_create(SEMIA_KEY_FILE, 3, 119);

    if(!set_semvalue(semia_id, 0, 1)){                      //mutex = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    if(!set_semvalue(semia_id, 1, 1)){                      //empty = 1
        perror("Could not set value of second semaphore");
        exit(4); 
    }

    if(!set_semvalue(semia_id, 2, 0)){                      //full = 0
        perror("Could not set value of second semaphore");
        exit(4); 
    }

                            /* PROCESSES */

    pid_t pid;

    //// Construction ////

    for(int i = 1;i <= 3; i++){
        pid = fork();
        if(pid == 0){
            srand(time(NULL)+i*150); //random
            Construction(componentsNumber, i);
            //printf("Construction of all Components of type %d Complete.\n\n\n", i);
            exit(EXIT_SUCCESS);
        }
        else if(pid < 0){
            perror("Fork failed");
        }
    }


    //// PaintShop ////

    pid = fork();
    if(pid == 0){
        PaintShop(3 * componentsNumber);
        //printf("Painting Complete.\n\n\n");
        exit(EXIT_SUCCESS);
    }
    else if(pid < 0){
        perror("Fork failed");
    }


    //// Inspection ////

    for(int i = 1;i <= 3; i++){
        pid = fork();
        if(pid == 0){
            Inspection(componentsNumber, i);
            //printf("Inspection of all Components of type %d Complete.\n\n\n", i);
            exit(EXIT_SUCCESS);
        }
        else if(pid < 0){
            perror("Fork failed --> Inspection");
        }
    }


    //// Assembly ////

    pid = fork();
    if(pid == 0){
        Assembly(3 * componentsNumber);
        //printf("Assembly Complete.\n\n\n");
        exit(EXIT_SUCCESS);
    }
    else if(pid < 0){
        perror("Fork failed --> Assembly");
    }

    for(int i = 1; i <= 8; i++){
        wait(NULL);
    }


                            /* DELETION */

    sem_delete(SEMCP_KEY_FILE);
    sem_delete(SEMPITYPE1_KEY_FILE);
    sem_delete(SEMPITYPE2_KEY_FILE);
    sem_delete(SEMPITYPE3_KEY_FILE);
    sem_delete(SEMIA_KEY_FILE);
    shm_delete(SHMCP_KEY_FILE, sizeof(component));
    shm_delete(SHMPITYPE1_KEY_FILE, sizeof(component));
    shm_delete(SHMPITYPE2_KEY_FILE, sizeof(component));
    shm_delete(SHMPITYPE3_KEY_FILE, sizeof(component));
    shm_delete(SHMIA_KEY_FILE, sizeof(component));

    exit(0);
};