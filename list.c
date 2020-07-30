#include <stdio.h>
#include <stdlib.h>
#include "list.h"


void listInit(list* List){

    List->head = NULL;
    List->tail = NULL;

}


node* inList(list* List, int pageNo, int processIndex){

    node* temp = List->head;
    while(temp != NULL){
        if(temp->pageNumber == pageNo && temp->process == processIndex){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}


int listInsert(list* List, int pageNo, char command, int processIndex){

    node* temp = inList(List, pageNo, processIndex);

    if(temp == NULL){
        temp = malloc(sizeof(node));
        temp->pageNumber = pageNo;
        if(command == 'W')
            temp->dirty = 1;
        else
            temp->dirty = 0;
        temp->process = processIndex;
        temp->next = NULL;

        if(List->head == NULL && List->tail == NULL){
            List->head = temp;
            List->tail = temp;
            return 1;
        }
        else{
            List->tail->next = temp;
            List->tail = temp;
            return 1;
        }
    }
    else{
        if(command == 'W')
            temp->dirty = 1;
            return 0;
    }
}


int listDeleteAll(list* List, int* count){

    int dirties = 0;
    node* delNode = List->head;
    node* temp;

    while(delNode != NULL){
        temp = delNode->next;
        if(delNode->dirty == 1)
            dirties++;
        free(delNode);
        (*count)++;
        delNode = temp;
    }

    List->head = NULL;
    List->tail = NULL;

    return dirties;
}

int listDeleteProcessEntries(list* List, int* count, int processIndex){

    int dirties = 0;
    node* delNode = List->head;
    node* previous = NULL;
    node* current = NULL;
    node* temp;

    while(delNode != NULL){
        if(delNode->process == processIndex){
            if(delNode->dirty == 1)
                dirties++;
            (*count)++;

            if(delNode == List->head){
                if(delNode == List->tail){
                    List->head = NULL;
                    List->tail = NULL;
                    free(delNode);
                    delNode = NULL;
                }
                else{
                    List->head = delNode->next;
                    current = List->head;
                    free(delNode);
                    delNode = current;
                }
            }
            else if(delNode == List->tail && delNode != List->head){
                previous->next = NULL;
                List->tail = previous;
                free(delNode);
                delNode = NULL;                
            }
            else{
                current = delNode->next;
                previous->next = current;
                free(delNode);
                delNode = current; 
            }
        }
        else{
            previous = delNode;
            delNode = delNode->next;
        }
    }
    return dirties;
}