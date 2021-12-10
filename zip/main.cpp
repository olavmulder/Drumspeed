#include "../SPI/spi.h"
#include "../CURL/curlData.h"
#include "../BACKUP/backup.h"
#include <ctype.h>
#include <error.h>
#include <unistd.h>
#include <time.h>
#include <thread>
#include <iostream>
#define DEBUG
#define TIMER1         4000000.0F
#define MAX_TIMER_VALUE 65536.0F
#define PI 3.14159265359f
#define BACKUP


float CalculateAvgTimeShort(uint16_t* counter, uint16_t* timer, uint8_t amount);
float CalculateAvg(float* array, uint8_t amount);
void CalculateSpeed(float *time, float* radius);
float CalculateRPM(float time);
int mainThread(int argc, char **argv);
void backupThread(float*, float*, float*, char*);

const float timer1time = (MAX_TIMER_VALUE/TIMER1);
const float timer1MaxValue = MAX_TIMER_VALUE;

int main(int argc, char **argv){
    std::thread main(mainThread, argc, argv);
    main.join();
    
    return 0;
}
int mainThread(int argc, char **argv){
    openlog("DrumSpeedSensor5.0 with parrallel thread", LOG_PERROR, LOG_SYSLOG);
    if(argc< 4){
        syslog(LOG_NOTICE, "ipv4 addr+server location, speed & diameter is needed");
        return 1;
    }
    /*spi values*/

    uint32_t speed = (uint32_t)atoi(argv[2]);
    //uint8_t timerValue = 5; 
    char *ipAddr = argv[1];
    float radius = (float)((float)atof(argv[3])/2.0f);
    uint8_t mode = 0;
    uint8_t  bits = 8;
    uint8_t askBytes = BEGIN_SEND;
    uint8_t end = END_SEND;
    uint8_t amountToRead;
    uint8_t data;
    uint8_t arrayDepth = 4;
    uint8_t **dataArray;
    spi_t spi;
    syslog(LOG_NOTICE, "IPV4: %s, radius: %.4f", ipAddr, radius);
    spi_init(&spi, name, mode, bits, speed);

    /*general values for for-loops etc.*/
    unsigned int i,j = 0;
    uint8_t k;
    /*intitTimer*/
    //TimerInit(timerValue);

    float timeToUseInDatabase[6];
    float rpmToUseInDatabase[6];
    uint8_t timeDatabaseIndex = 0;/*index for holding location in array*/
    int amountToReadBuffer;
    //Timer0Start();



    while(1){
        clock_t begin = clock();
        for(k=0;k<6;k++){
            /*SPITransfer();*/
            clock_t beginMain = clock();
            spi_write(&spi, &askBytes, sizeof(askBytes));
            usleep(1000);
            
            spi_read(&spi,&amountToRead, sizeof(amountToRead));
           
            
            dataArray= (uint8_t**)malloc(amountToRead * sizeof(uint8_t*));
            for(i=0;i<amountToRead;i++){
                dataArray[i] = (uint8_t*)malloc(arrayDepth * sizeof(uint8_t));
                
            }
            
            uint16_t *counter;
            counter = (uint16_t*)malloc(amountToRead * sizeof(uint16_t));

            uint16_t *timer;
            timer = (uint16_t*)malloc(amountToRead * sizeof(uint16_t));

            for(i=0;i<amountToRead;i++){
                for(j=0;j<arrayDepth;j++){
                    spi_read(&spi, &data, sizeof(data));
                    *(*(dataArray+i)+j) = data;
                    
                }
                
            } 
            spi_write(&spi, &end, sizeof(end));
            /*--end spi--*/
            /*for(i=0;i<amountToRead;i++){
                for(j=0;j<arrayDepth;j++){
                    printf("dataArray[%d][%d] = %d\n", i,j,*(*(dataArray+i)+j));
                }
            }*/
            /*--begin calculation time--*/
            for(i=0;i<amountToRead;i++){
                
                *(timer+i) =   (uint16_t)( ((uint16_t) *(*(dataArray+i)+3) << 8) | ( *(*(dataArray+i) +2) << 0));
                *(counter+i) = (uint16_t)( ((uint16_t) *(*(dataArray+i)+1) << 8) | ( *(*(dataArray+i) +0) << 0));
                
            }
            /*avg time used for last 5 sec*/
            timeToUseInDatabase[timeDatabaseIndex] = CalculateAvgTimeShort(counter, timer,amountToRead);
            //syslog(LOG_NOTICE, "timeToUseInDatabase %f", timeToUseInDatabase[timeDatabaseIndex]);
            rpmToUseInDatabase[timeDatabaseIndex] = CalculateRPM(timeToUseInDatabase[timeDatabaseIndex]);
            //syslog(LOG_NOTICE, "amount: %d, avg time par detect: %f, rpm: %f", amountToRead, timeToUseInDatabase[timeDatabaseIndex], rpmToUseInDatabase[timeDatabaseIndex]);

            timeDatabaseIndex ++;
            /*free data array*/
            for(i=0;i<amountToRead;i++){
                free(dataArray[i]);
            } 
            free(timer);
            free(counter);
            //syslog(LOG_NOTICE, "\n\n");
            amountToReadBuffer += amountToRead;
            clock_t endMain;
            double timeSpent;
            do{
                endMain = clock();
                timeSpent = (double)(endMain-beginMain) / CLOCKS_PER_SEC;
            }while(timeSpent < 4.95000);

            //while(!TimerIRQPending())continue;
            //clock_t endMainPlusTimer = clock();
            //printf("timer main: %f\n", timeSpent);
        }
        /*after 6 times send data to database*/  
       
        /*avg time of all data*/
        float speed = CalculateAvg(timeToUseInDatabase, timeDatabaseIndex);
        float rpm = CalculateAvg(rpmToUseInDatabase,timeDatabaseIndex);
        float amount;
        if(amountToReadBuffer == 0)amount = 0.0f;
        else amount = (float)amountToReadBuffer;
        amountToReadBuffer = 0;
        syslog(LOG_INFO, "send to database, speed %f, rpm %f, amount: %f", speed, rpm, amount);

        /*calculate speed*/
        CalculateSpeed(&speed, &radius);
        /*send speed*/
        
        std::thread backup(backupThread, &speed, &rpm, &amount, &ipAddr[0]);
        backup.detach();

        timeDatabaseIndex = 0;
        double time_spent;
        do{
            clock_t end = clock();
            time_spent = (double)(end-begin) / CLOCKS_PER_SEC;
        }while(time_spent < 30.0000);
        //printf("totaal time is %f\n", time_spent);
    }
    spi_free(&spi);
    free(ipAddr);
    closelog();
    
}
void backupThread(float* speed, float* rpm, float* amount, char* addr){
    unsigned int i;
    char* timeString;
    //get time
    time_t t = time(NULL);
    struct tm  *tm = localtime(&t);
    timeString = (char*)malloc(sizeof(char)*20);
    snprintf(timeString, 20, "%04d-%02d-%02d %02d:%02d:%02d", tm->tm_year+1900,tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    
    float **array;
    char timeArray[20];
    char *timeArr;


    //get size
    unsigned int sizeBackupList = GetSizeList();
    //if there is data in backup list
    if(sizeBackupList > 0){
            //connect & ping
            if(!system("/bin/sh networkscript.sh")){//if succeeds send all!
                //make array to store all data
                array = (float**)malloc(sizeBackupList * sizeof(float*));
                for(i=0;i<sizeBackupList;i++){
                    array[i] = (float*)malloc(3 * sizeof(float)); 
                }
                timeArr = (char*)malloc(sizeBackupList * sizeof(timeArray));
                
                //put all data in sizeBackupList
                BackupGetAllData(array, timeArr, sizeBackupList);
                //send all data in array to Database
                unsigned int b;
                for(b=0;b<sizeBackupList;b++){
                    //strcpy(timeArray, (timeArr+i));
                    for(uint8_t j = 0;j<20;j++){
                        timeArray[j] = *((timeArr+j)+b*20);
                    }       
                    timeArray[19] = '\0';
                    if(SendDataToDatabase((*(array+b)+0), (*(array+b)+1), addr, (*(array+b)+2), &timeArray[0], 1)){//not sended
                        BackupData((*(array+b)+0), (*(array+b)+1), (*(array+b)+2), &timeArray[0]);
                    }
                }
                //send last measured data
                if(SendDataToDatabase(speed,rpm,addr,amount,timeString, 0)){//not sended
                    unsigned int sizeBackupList = BackupData(speed, rpm, amount, timeString);
                    syslog(LOG_NOTICE, "Failed to send data, size backup list is %d\n", sizeBackupList);
                }
                free(array);
                free(timeArr);
                

            }else{//there is data but still not a connection, backup the data
                unsigned int sizeBackupList = BackupData(speed, rpm, amount, timeString);
                syslog(LOG_NOTICE, "Failed to send data, size backup list is %d\n", sizeBackupList);

            }
    }else{//no data in backup list
        if(SendDataToDatabase(speed,rpm, addr, amount, timeString, 0)){//send data to database,
            //failed so backup the data
            unsigned int sizeBackupList = BackupData(speed, rpm, amount, timeString);
            syslog(LOG_NOTICE, "Failed to send data, size backup list is %d\n", sizeBackupList);
        }
    }
    free(timeString);
}
float CalculateAvg(float* array, uint8_t amount){
    /*caluculate avg time, if value is inf, than trow it away*/
    float total=0;
    uint8_t i=0;
    
    uint8_t amountBuffer = amount;
    for(i=0;i<amount;i++){
        if(*(array+i) == 0){
            amountBuffer--;
        }else{
            total+= *(array+i);
        }
    }
    if(total ==0 || amountBuffer == 0){
        return 0;
    }
    total = total/(float)amountBuffer;
    
    return total;

   
}

float CalculateAvgTimeShort(uint16_t *counter, uint16_t* timer, uint8_t amount){
    float total=0;
    uint8_t i=0;
    uint8_t amountBuffer = amount;
    for(i=0;i<amount;i++){
        if(*(counter+i) == 0 && *(timer+i) == 0){
            amountBuffer --;
        }else{
            //syslog(LOG_INFO, "counter = %f", (float) *(counter+i) );
            //syslog(LOG_INFO, "timer time = %f", (float)   (( (*(timer+i)+1)  /   timer1MaxValue ) * timer1time));
            
            total += (float)   (*(counter+i)    * timer1time);
            total += (float)(( (*(timer+i)+1)  /   timer1MaxValue ) * timer1time);
        }
        
    }
    if(amountBuffer == 0){
        //syslog(LOG_NOTICE, "amountBuffer in CaluclateAvgTimeShort = 0 totaal = %f", total);
        return 0;
    }else{
        total = total/(float)amountBuffer;
        //syslog(LOG_NOTICE, "total time/amount:  %f/%d = %f",total*amountBuffer,amountBuffer, total);
        return total;
    }
}
void CalculateSpeed(float* speed, float* radius){
    if(*speed == 0){
        return;
    }
    float distance = (float)2*(*radius)*PI;
    //printf("distance = %f\n", distance);
    //printf("time %f\n", *speed);
    *speed = distance/(*speed);
    //printf("speed %f\n", *speed);
    
}

float CalculateRPM(float time){
    if( time == 0.0)
        return 0;
    else{
        //syslog(LOG_NOTICE, "return value in CalcRPM = %f", (float)60.0/time);
        return ((float)60.0/time);
    }
    
}