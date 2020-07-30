#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "keys.h"
#include "Semaphores.h"
#include "Sem_shm_fun.h"
#include "list.h"
#include "hashTable.h"
#include "functions.h"

int main(int argc, char* argv[]){

    if(argc < 4 || argc > 5){
        printf("Invalid Input!\n");
        return -1;
    }

    int k = atoi(argv[1]);
    int frames = atoi(argv[2]);

    if( k > frames / 2){
        printf("Invalid k/frames values!");
        return -1;
    }
    int q = atoi(argv[3]);
    int max;

    if(argc == 5)
        max = atoi(argv[4]);
    else
        max = 1000000;

    // Shared memory segment initialization //

     int shm_id = shm_create(SHM_KEY_FILE, 2 * q * sizeof(refTrace), 32);

    // Semaphores initialization //

    int sem1_id = sem_create(SEM1_KEY_FILE, 3, 470);
    //printf("sem_id_1: %d\n", sem1_id);

    
    if(!set_semvalue(sem1_id, 0, 1)){                      //mutex = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    if(!set_semvalue(sem1_id, 1, 1)){                      //empty = 1
        perror("Could not set value of second semaphore");
        exit(4); 
    }

    if(!set_semvalue(sem1_id, 2, 0)){                      //full = 0
        perror("Could not set value of second semaphore");
        exit(4); 
    }

    /////////////////////////////////////////////////////////////////////

    int sem2_id = sem_create(SEM2_KEY_FILE, 3, 256);
    //printf("sem_id_2: %d\n", sem2_id);

    if(!set_semvalue(sem2_id, 0, 1)){                      //mutex = 1
        perror("Could not set value of first semaphore");
        exit(4);
    }

    if(!set_semvalue(sem2_id, 1, 1)){                      //empty = 1
        perror("Could not set value of second semaphore");
        exit(4); 
    }

    if(!set_semvalue(sem2_id, 2, 0)){                      //full = 0
        perror("Could not set value of second semaphore");
        exit(4);
    }

    ///////////////////////////* PROCESSES */////////////////////////////

    // PM1, PM2 //

    pid_t pidpm, pidmm;

    for(int i = 1; i < 3; i++){
        pidpm = fork();
        if(pidpm == 0){
            PM(i, q, max);
            exit(i);
        }
        else if(pidpm < 0){
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
    }

    // MM //

    pidmm = fork();
    if(pidmm == 0){
        MM(q, k, frames, max);
        exit(3);
    }
    else if(pidmm < 0){
        perror("Fork failed");
            exit(EXIT_FAILURE);
    }

    for(int i = 0; i < 3; i++){
        wait(NULL);
    }

    // Semaphores & Shared Memory Segments Deletion //

    sem_delete(SEM1_KEY_FILE);
    sem_delete(SEM2_KEY_FILE);
    shm_delete(SHM_KEY_FILE, 2 * q * sizeof(refTrace));

    exit(EXIT_SUCCESS);
}