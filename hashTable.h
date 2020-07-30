#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct refTrace{
    unsigned int pageNumber;
    char command;
}refTrace;

typedef struct hashTable{
    struct list** listArray;
    int numBuckets;
    int count;
    int totalEntries;
    int totalReads;
    int totalWrites;

}hashTable;

void createTrace(struct refTrace*, unsigned int, char);
unsigned int hashFunction(unsigned int, int);
hashTable* hashCreate(int);
int hashInsert(struct refTrace, hashTable*, int);
void hashEmpty(hashTable*,int);
void hashDelete(hashTable*);

#endif