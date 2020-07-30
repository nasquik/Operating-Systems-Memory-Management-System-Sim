#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"
#include "hashTable.h"


void createTrace(struct refTrace* trace, unsigned int pageNo, char command){

    trace->pageNumber = pageNo;
    trace->command = command;
}


unsigned int hashFunction(unsigned int pageNo, int numBuckets){

    long p = 1302397;
    long a = 35759;
    long b = 128932;
    
    return ((a * pageNo + b) % p) % numBuckets;
}


hashTable* hashCreate(int numbuckets){

    hashTable* HT = malloc(sizeof(hashTable));

    HT->numBuckets = numbuckets;
    HT->count = 0;
    HT->totalEntries = 0;
    HT->totalReads = 0;
    HT->totalWrites = 0;
    HT->listArray = malloc(numbuckets * sizeof(list*));
    for(int i = 0; i < numbuckets; i++){
        HT->listArray[i] = malloc(sizeof(list));
        listInit(HT->listArray[i]);
    }
    printf("Hash Table Created\n");

    return HT;
}


int hashInsert(refTrace trace, hashTable* HT, int processIndex){

    int position = hashFunction(trace.pageNumber, HT->numBuckets);

    int pageNo = trace.pageNumber;
    char commmand = trace.command;

    int reads = listInsert(HT->listArray[position], pageNo, commmand, processIndex);
    HT->totalReads += reads;

    if(reads){
        HT->count++;
        HT->totalEntries++;
        printf("Page No %x added to Hash Table by PM%d\n", pageNo, processIndex);
        return 1;
    }

    printf("Page No %x updated by PM%d\n", pageNo, processIndex);
    return 0;
}


void hashEmpty(hashTable* HT, int processIndex){
    int ccount = 0;
    int writes = 0;
    for(int i = 0; i < HT->numBuckets; i++){
        writes += listDeleteProcessEntries(HT->listArray[i], &ccount, processIndex);
    }
    HT->totalWrites += writes;
    HT->count -= ccount;
    printf("Number of Entries currently in Hash Table: %d\n", HT->count);
    printf("Flushed all Entries made by process %d\n", processIndex);
}


void hashDelete(hashTable* HT){

    int count;
    for(int i = 0; i < HT->numBuckets; i++){
        listDeleteAll(HT->listArray[i], &count);
        free(HT->listArray[i]);
        HT->count = HT->count - count;
    }
    free(HT->listArray);
    free(HT);
    printf("Hash Table Deleted\n");
}