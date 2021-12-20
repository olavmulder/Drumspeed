#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
struct Node{
    float rpm;
    float speed;
    float amount;
    char time[20];
    struct Node *next;
};

void InsertFirst(float *speed, float*rpm, float* amount, char* time);
void InsertLast(float *speed, float*rpm, float* amount, char* time);
float* DeleteFirst();/*returned data*/
char* GetTime();
void PrintList();
unsigned int GetSizeList();
bool listEmpty();

#endif