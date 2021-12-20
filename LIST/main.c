#include "linkedList.h"


int main(){
    float *speed, *rpm, *amount;
    char *time;
    float _speed = 0.10f, _rpm  = 0.10f, _amount = 0.10f; 
    char _time = 'a';
    speed = &_speed;
    rpm = &_rpm;
    amount = &_amount;
    time = &_time;
    for(;;){
        for(int i=0;i< 24*60*2;i++){
            if(listEmpty()){
                InsertFirst(speed, rpm, amount, time);
            }else{
                InsertLast(speed, rpm, amount, time);
            }  
            _speed = _rpm = _amount += 0.10f;
        }
        PrintList();
        printf("\n");
        int size = GetSizeList();
        for(int i=0;i<size;i++){
            DeleteFirst();
            //PrintList();
        }
        PrintList();
    }
    
   

}