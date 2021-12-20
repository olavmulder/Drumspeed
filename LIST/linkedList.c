#include "linkedList.h"
static float returnArray[3];
static char returntimeArray[20];

static struct Node *beginHead;
static struct Node *head;
static struct Node *tail;
static size_t size;
/*
 *list = calloc 24*60*2 node struct; 
 * head = list
 * last ++
 * 
 */

void InsertFirst(float *speed, float *rpm, float *amount, char* time){
    head = (struct Node*)malloc(24*60*2*sizeof(struct Node));
    if(!head)printf("malloc head\n"); 
    beginHead = head;
    head->speed = *speed;
    head->rpm = *rpm;
    head->amount = *amount;
    strcpy(head->time, time);
    head->next = NULL;
    tail = head;
    size = 1;
}
void InsertLast(float*speed, float*rpm, float*amount, char* time){
    tail->next = tail+1;
    tail = tail+1;
    size +=1;
    if(size == 24*60*2-1){
        printf("size to big??\n");
        exit(1);
    } 
    tail->speed = *speed;
    tail->rpm = *rpm;
    tail->amount = *amount;
    strcpy(tail->time, time);
    tail->next = NULL;
}
unsigned int GetSizeList(){
    return size;
}
char* GetTime(){
    strcpy(returntimeArray,&head->time[0]);
    return &returntimeArray[0];
}
float* DeleteFirst(){
    
    returnArray[0] =  head->speed;
    returnArray[1] =  head->rpm;
    returnArray[2] =  head->amount;
    if(head == NULL){
        head = beginHead;
    }else{
        head = head->next;
    }
    size--;
    if(size == 0){
        free(head);
    }
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
