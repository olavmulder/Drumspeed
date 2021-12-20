#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(){
    printf("begin\n");
    int i=0;
    while(1){
        clock_t begin = clock();
        float time;
        do{
            time = (double)(clock()-begin)/CLOCKS_PER_SEC;
        }while(time < 5.56848);
        printf("time %.7f\n", time);
        
    }
}