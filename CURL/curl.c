#include "curlData.h"
const size_t standLenght = 5*8;
int SendDataToDatabase(float *_speed, float*_rpm, char *ipAddr, float* _amountToRead, char* time, char backup)
{

    //get lenght speed
    size_t lenData = (size_t)snprintf(NULL, 0, "%.4f", *_speed);
    char *speed = (char*)malloc(lenData);
    snprintf(speed, lenData+1, "%.6f", *_speed);
    //get lenght rpm
    lenData = (size_t)snprintf(NULL, 0, "%.4f", *_rpm);
    char *rpm = (char*)malloc(lenData);
    snprintf(rpm, lenData+1, "%.4f", *_rpm);
    //get lenght amount
    lenData = (size_t)snprintf(NULL,0, "%.4f", *_amountToRead);
    char *amountToRead = (char*)malloc(lenData);
    snprintf(amountToRead, lenData+1,"%.4f", *_amountToRead);
    //get lenght whole string
    size_t length = standLenght +  strlen(ipAddr) + strlen(speed) \
        + strlen(rpm) + strlen(amountToRead)+2;
    //printf("backup: %c\n", backup);
    char *finleString;
    finleString = (char*)malloc(length);
    sprintf(finleString, "%s%s%.4f%s%.4f%s%.4f%s%s%c", ipAddr, standaardInfoPart1, *_speed, standaardInfoPart2, *_rpm, \
        standaardInfoPart3,*_amountToRead,standaardInfoPart4, standaardInfoPart5, backup);
    //printf("%s\n", finleString);
    time = time;
   

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if(curl) {
        
        curl_easy_setopt(curl, CURLOPT_URL, finleString);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            free(finleString);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return 1;
        }
        curl_easy_cleanup(curl);   
    }
    free(finleString);
    curl_global_cleanup();
    return 0;
}
