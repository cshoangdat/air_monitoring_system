#ifndef __COMMON__
#define __COMMON__

#include "stdint.h"
#include "config.h"

#define FW_VERSION_STR MKSTR(FW_VERSION_MAJOR)\
                       MKSTR(FW_VERSION_MINOR)\
                       MKSTR(FW_VERSION_BUILD)

#define MAC_CEN              "10:91:A8:01:5D:E0"
#define MAC_1                "EC:DA:3B:AB:04:C8"  
#define MAC_2                "EC:DA:3B:AB:0B:A8"  

typedef struct
{
    char* WifiName;
    char* WifiPass;
    char DevIP[20];
}WifiData_t;

WifiData_t WifiData;

typedef struct
{
    bool isDev1;
    bool isDev2;
    bool isDevCen;
    bool isWifiConnected;
    bool isI2cInit;
    bool isSht4xInit;
    bool isBmp280Init;
    bool isSpiInit;
    bool isSgp30Init;
    bool isLoraRev;
    bool isNextion;
}setUp_t;

setUp_t setUp;

typedef struct
{
    float shtTemp;       
    float shtHumid;      
    float bmp280Pressure;
    float bmp280Temp;    
    float bmp280Humid;   
    uint16_t sgp30Co2;  
    uint16_t sgp30Tvoc;
    uint8_t dataSensorRev[256];  
}sensorData_t;

sensorData_t sensorData;

typedef struct
{
    uint8_t* pageDev0;
    uint8_t* pageDev1;
    uint8_t* pageDev2;
    uint8_t* pageDev3;
}pageDev_t;

pageDev_t pageDev;

typedef struct
{
    bool isUpdate;
}OTA_t;

OTA_t OTA;

#endif
