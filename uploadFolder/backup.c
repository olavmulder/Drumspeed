#include "backup.h"

unsigned int BackupData(float *speed, float* rpm, float* amount, char* time){
    if(listEmpty()){
        InsertFirst(speed, rpm, amount, time);
        
    }else{
        InsertLast(speed, rpm, amount, time);
    }
    return GetSizeList();
}
void BackupGetAllData(float **array, char *timeArray, int sizeLinkedList){
    int i;
    float* addr;
    char* time;
    for(i=0;i< sizeLinkedList; i++){ 
        time = GetTime();
        strcpy((timeArray+i*20), time);
        
        addr = DeleteFirst();
        *(*(array+i))   = *(addr);
        *(*(array+i)+1) = *(addr+1);
        *(*(array+i)+2) = *(addr+2);
    }
}