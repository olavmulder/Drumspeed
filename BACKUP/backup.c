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
        /*for(uint8_t j = 0;j<20;j++){
            printf("%c", *((timeArray+j)+i*20));
        }
        printf("\n");*/
        addr = DeleteFirst();
        *(*(array+i))   = *(addr);
        *(*(array+i)+1) = *(addr+1);
        *(*(array+i)+2) = *(addr+2);
    }
}