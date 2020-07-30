#ifndef LIST_H
#define LIST_H

typedef struct node{
    int pageNumber;
    char dirty;
    int process;
    struct node* next;
}node;

typedef struct list{

    struct node* head;
    struct node* tail;
}list;

void listInit(list*);
int listInsert(list*, int, char, int);
int listDeleteAll(list*, int*);
int listDeleteProcessEntries(list*, int*, int);
node* inList(list*, int, int);

#endif