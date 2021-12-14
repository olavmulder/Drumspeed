#include "curlData.h"
int SendDataToDatabase(float *_speed, float*_rpm, char *ipAddr, float* _amountToRead, char* time, char backup)
{

    int i=0;
    
    size_t lenData = (size_t)snprintf(NULL, 0, "%.4f", *_speed);
    char *speed = (char*)malloc(lenData);
    snprintf(speed, lenData+1, "%.6f", *_speed);

    lenData = (size_t)snprintf(NULL, 0, "%.4f", *_rpm);
    char *rpm = (char*)malloc(lenData);
    snprintf(rpm, lenData+1, "%.4f", *_rpm);

    lenData = (size_t)snprintf(NULL,0, "%.4f", *_amountToRead);
    char *amountToRead = (char*)malloc(lenData);
    snprintf(amountToRead, lenData+1,"%.4f", *_amountToRead);

    size_t length = strlen(standaardInfoPart1)+strlen(standaardInfoPart2)+ \
        strlen(standaardInfoPart3)+strlen(standaardInfoPart4)+ \
        strlen(standaardInfoPart5)+ strlen(standaardInfoPart6) + strlen(time) + strlen(amountToRead)+ \
        strlen(rpm)+strlen(speed)+1;
    char *finleString;
    finleString = (char*)malloc(length+1);
    
    for(i=0;i<(int)length+1;i++){
        finleString[i] = '\0';
    }

    strcat(finleString,standaardInfoPart1);
    strcat(finleString,speed);
    strcat(finleString, standaardInfoPart2);
    strcat(finleString,rpm);
    strcat(finleString, standaardInfoPart3);
    strcat(finleString,amountToRead);
    strcat(finleString,standaardInfoPart4);
    strcat(finleString, time);
    strcat(finleString, standaardInfoPart5);
    strcat(finleString, &backup);
    strcat(finleString, standaardInfoPart6);
    
    free(speed);
    free(rpm);
    free(amountToRead);
   
    lenData = (size_t)snprintf(NULL,0, "%s", ipAddr);
    char ipAddress[lenData];
    for(i=0;i<(int)lenData+1;i++){
        ipAddress[i] = '\0';
    }
    strcat(ipAddress, ipAddr);
    CURL *curl;
    CURLcode res;
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, ipAddress);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, finleString);
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