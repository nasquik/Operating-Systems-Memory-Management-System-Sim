#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include "keys.h"
#include "Semaphores.h"
#include "Sem_shm_fun.h"
#include "list.h"
#include "hashTable.h"


int PM(int index, int q, int max){

    int shm_id = shm_recover(SHM_KEY_FILE, 2 * q * sizeof(refTrace));
    refTrace* array = (refTrace*)shmat(shm_id, 0, 0);

    refTrace* segment = NULL;

    int sem_id;
    
    FILE* file = NULL;

    switch(index){
        case 1:
            sem_id = sem_recover(SEM1_KEY_FILE);
            //printf("sem_id_1: %d\n", sem_id);
            file = fopen("gcc.trace", "r");
            segment = array;
            break;
        case 2:
            sem_id = sem_recover(SEM2_KEY_FILE);
            //printf("sem_id_2: %d\n", sem_id);
            file = fopen("bzip.trace", "r");
            segment = array + q;
            break;
        default:
            printf("Invalid PM index\n");
            exit(EXIT_FAILURE);
    }

    if(file == NULL){
        printf("Error opening file\n");
        exit(EXIT_FAILURE);
    }

    char* line = NULL;
    size_t len = 0;
    unsigned int hex = 0;
    char command = 0;
    int totalCount = 0;
    int counter = 0;
    int flag = 0;
    refTrace trace = {};

    while(totalCount < max){

        if(!semaphore_p(sem_id, 1))       //down(empty)
            exit(EXIT_FAILURE);

        if(!semaphore_p(sem_id, 0))       //down(mutex)
            exit(EXIT_FAILURE);
        
        while(counter < q && totalCount < max && getline(&line, &len, file) != -1){
            sscanf(line, "%x %c",  &hex, &command);
            hex /= 4096;
            createTrace(&trace, hex, command);
           // printf("index: %d &&& pageNo: %x\n", index, trace.pageNumber);
            segment[counter] = trace;

            counter++;
            totalCount++;
        }

        if(!semaphore_v(sem_id, 0))       //up(mutex)
            exit(EXIT_FAILURE);

        if(!semaphore_v(sem_id,2))        //up(full)
            exit(EXIT_FAILURE);
    
        counter = 0;

    }
    
    shmdt((void *)array);

    fclose(file);
    if (line)
        free(line);
    
    exit(EXIT_SUCCESS);
}

int MM(int q, int k, int frames, int max){

    int shm_id = shm_recover(SHM_KEY_FILE, 2 * q * sizeof(refTrace));
    refTrace* array = (refTrace*)shmat(shm_id, 0, 0);

    int sem1_id = sem_recover(SEM1_KEY_FILE);
    int sem2_id = sem_recover(SEM2_KEY_FILE);

    int MaxFrames = 0;
    int totalCount = 0;
    int totalCount1 = 0;
    int totalCount2 = 0;
    int PFcount1 = 0;
    int PFcount2 = 0;
    int position = 0;
    int BucketsNo = frames / 3;

    hashTable* table = hashCreate(BucketsNo);

    while(totalCount1 < max){

        if(!semaphore_p(sem1_id, 2))       //down(full)
            exit(EXIT_FAILURE);
        
        if(!semaphore_p(sem1_id, 0))       //down(mutex)
            exit(EXIT_FAILURE);

        for(int i = 0; i < q; i++){

            if(totalCount1 == max)
                break;

            if(PFcount1 < k){
                if(hashInsert(array[i], table, 1))
                    PFcount1++;
            }
            else if(PFcount1 == k){
                position = hashFunction(array[i].pageNumber, BucketsNo);
                if(!inList(table->listArray[position], array[i].pageNumber, 1)){
                    printf("Flush all Entries made by Process 1\n");
                    hashEmpty(table, 1);
                    PFcount1 = 0;
                    hashInsert(array[i], table, 1);
                    PFcount1++;
                }
                else{
                    hashInsert(array[i], table, 1);
                }
                if(PFcount1 + PFcount2 > MaxFrames){
                    MaxFrames = PFcount1 + PFcount2;
                }
            }
            totalCount1++;
        }

        if(!semaphore_v(sem1_id, 0))       //up(mutex)
            exit(EXIT_FAILURE);

        if(!semaphore_v(sem1_id,1))        //up(empty)
            exit(EXIT_FAILURE);

        //////////////////////////////////

        if(!semaphore_p(sem2_id, 2))       //down(full)
            exit(EXIT_FAILURE);
        
        if(!semaphore_p(sem2_id, 0))       //down(mutex)
            exit(EXIT_FAILURE);

        for(int i = q; i < 2 * q; i++){

            if(totalCount2 == max)
                break;

            if(PFcount2 < k){
                if(hashInsert(array[i], table, 2))
                    PFcount2++;
            }
            else if(PFcount2 == k){
                position = hashFunction(array[i].pageNumber, BucketsNo);
                if(!inList(table->listArray[position], array[i].pageNumber, 2)){
                    printf("Flush all Entries made by Process 1\n");
                    hashEmpty(table, 2);
                    PFcount2 = 0;
                    hashInsert(array[i], table, 2);
                    PFcount2++;
                }
                else{
                    hashInsert(array[i], table, 2);
                }
                if(PFcount1 + PFcount2 > MaxFrames){
                    MaxFrames = PFcount1 + PFcount2;
                }
            }
            totalCount2++;
        }

        if(!semaphore_v(sem2_id, 0))       //up(mutex)
            exit(EXIT_FAILURE);

        if(!semaphore_v(sem2_id,1))        //up(empty)
            exit(EXIT_FAILURE);
    }

    printf("\nStatistics before the Hash Table is Deleted:\n\n");

    printf("Total Disk Reads: %d\n", table->totalReads);
    printf("Total Disk Writes: %d\n", table->totalWrites);
    printf("Total Page Faults: %d\n", table->totalEntries);
    printf("Total Traces Examined: %d\n", 2 * max);
    printf("Maximum Number of Frames in Use at Once: %d\n\n", MaxFrames);

    hashEmpty(table, 1);
    hashEmpty(table, 2);

    printf("\nStatistics after the Hash Table is Deleted:\n\n");

    printf("Total Disk Reads: %d\n", table->totalReads);
    printf("Total Disk Writes: %d\n", table->totalWrites);
    printf("Total Page Faults: %d\n", table->totalEntries);
    printf("Total Traces Examined: %d\n", 2 * max);
    printf("Maximum Number of Frames in Use at Once: %d\n\n", MaxFrames);

    hashDelete(table);
    shmdt((void *)array);
    
    exit(EXIT_SUCCESS);
}