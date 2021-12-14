#ifndef BACKUP_H
#define BACKUP_H
#include "../LIST/linkedList.h"
unsigned int BackupData(float *speed, float*rpm, float* amount, char* time);
void BackupGetAllData(float**array, char*timeArray, int sizeLinkedList);
#endif