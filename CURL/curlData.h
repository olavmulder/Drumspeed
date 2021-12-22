#ifndef CURL_H
#define CULR_H
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <curl/curl.h>
#include <string.h>
#include <float.h>

struct string{
    char *ptr;
    size_t len;
};
int SendDataToDatabase(float* _speed, float* _rpm, char * ipAddr, \
 float* _amountToRead, char* _time, char backup);

//static const char standaardInfoPart0[] = "?api_key=PUWI7VQCMC8BOSIT";
static const char standaardInfoPart1[] = "&field1=";
static const char standaardInfoPart2[] = "&field2=";
static const char standaardInfoPart3[] = "&field3=";
static const char standaardInfoPart4[] = "&field4=";
static const char standaardInfoPart5[] = "&field5=";

/*
static const char standaardInfoPart1[] = "speed=";
static const char standaardInfoPart2[] = "&rpm=";
static const char standaardInfoPart3[] = "&amount=";
static const char standaardInfoPart4[] = "&measurementTime=";
static const char standaardInfoPart5[] = "&backup=";
static const char standaardInfoPart6[] = "&username=olav&pwd=Ammeraal123!&database=testTable";
*/

#endif