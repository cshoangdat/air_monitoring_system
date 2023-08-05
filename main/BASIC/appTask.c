#include "appTask.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "cJSON.h"
#include "esp_netif.h"
#include "esp_event.h"

#include "../NVS/NVSDriver.h"
#include "../INCLUDE/common.h"
#include "../INCLUDE/config.h"
#include "../NEXTION/nextion.h"
#define TAG "AppTask"

void appSetUp(void){
    esp_err_t err = NVSDriverInit();
    if(err != ESP_OK){
        ESP_LOGE(TAG, "NVS Flash Init Failed");
    }
    err = esp_netif_init();
    if(err != ESP_OK){
        ESP_LOGE(TAG, "ESP NetIF Init Failed");
    }
    err = esp_event_loop_create_default();
    if(err != ESP_OK){
        ESP_LOGE(TAG, "ESP Create Event Failed");
    }

    setUp.isWifiConnected = false;
    if(NVSDriverReadString(NVS_NAMESPACE_CONFIG, NVS_KEY_WIFI_SSID, &WifiData.WifiName) != ESP_OK){
        WifiData.WifiName = (char* )DEFAULT_SSID;
    }
    if(NVSDriverReadString(NVS_NAMESPACE_CONFIG, NVS_KEY_WIFI_PASS, &WifiData.WifiPass) != ESP_OK){
        WifiData.WifiPass = (char* )DEFAULT_PASS;
    }

    initNextion();
    sendData("page 0\xFF\xFF\xFF");
    char* data_out = (char*)malloc(TX_BUF_SIZE);
    sprintf(data_out, "Setting.wifi_id.txt=\"%s\"\xFF\xFF\xFF",WifiData.WifiName);
    sendData(data_out);  
    sprintf(data_out, "Setting.wifi_pass.txt=\"%s\"\xFF\xFF\xFF",WifiData.WifiPass);
    sendData(data_out);
    sendData("restart.j0.val=0\xFF\xFF\xFF");
}