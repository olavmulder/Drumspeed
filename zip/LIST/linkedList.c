#include "linkedList.h"
static struct Node *head =NULL, *last =NULL;
static float returnArray[3];
static char returntimeArray[20];
void InsertFirst(float *speed, float *rpm, float *amount, char* time){
    struct Node *link = (struct Node*)malloc(sizeof(struct Node));
    link->speed = *speed;
    link->rpm = *rpm;
    link->amount = *amount;
    strcpy(link->time, time);
    //printf("speed %f, rpm %f, amount %f, time %s\n", link->speed, link->rpm, link->amount, link->time);
    if(listEmpty()){
        last = (struct Node*)malloc(sizeof(struct Node));
        last  = link;
        last->next = NULL;
    }
    link->next = head;
    head = link;
}
void InsertLast(float*speed, float*rpm, float*amount, char* time){
    struct Node *link = (struct Node*)malloc(sizeof(struct Node));
    link->speed = *speed;
    link->rpm = *rpm;
    link->amount = *amount;
    strcpy(link->time, time);
    link->next = NULL;

    last->next = link;
    last = link;
}
unsigned int GetSizeList(){
    struct Node* temp = head;
    unsigned int i=0;
    while(temp != NULL){
        i++;
        temp = temp->next;
    }
    return i;
}
char* GetTime(){
    strcpy(returntimeArray,&head->time[0]);
    return &returntimeArray[0];
}
float* DeleteFirst(){
    struct Node* temp  = head;
    returnArray[0] =  head->speed;
    returnArray[1] =  head->rpm;
    returnArray[2] =  head->amount;
    head = head->next;
    free(temp);
    return &returnArray[0];
}
void PrintList(){
    struct Node *temp = head;
    while(temp != NULL){
        printf("speed: %f, rpm: %f, amount: %f\n", temp->speed, temp->rpm, temp->amount);
        temp = temp->next;
    }
}

bool listEmpty(){
    if(head == NULL){
        return true;
    }else{
        return false;    
    }
}
