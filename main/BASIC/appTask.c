#include "appTask.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "cJSON.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"
#include "string.h"
#include "stdlib.h"
#include <stdio.h>

#include "../NVS/NVSDriver.h"
#include "../INCLUDE/common.h"
#include "../INCLUDE/config.h"
#include "../NEXTION/nextion.h"
#define TAG "AppTask"

static portMUX_TYPE param_lock = portMUX_INITIALIZER_UNLOCKED;

static char* ReadBaseMac(void){
    char* mac_str = (char*) malloc(18 * sizeof(char));
    int mac_len = esp_efuse_get_field_size(ESP_EFUSE_OPTIONAL_UNIQUE_ID);
    uint8_t mac[mac_len];
    esp_err_t err = esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, mac, sizeof(mac));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read MAC address from eFuse: %s", esp_err_to_name(err));
        return;
    }
    else{
        sprintf(mac_str, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        ESP_LOGI(TAG, "Base MAC: %s", mac_str); 
        return mac_str;   
    }
}

void appSetUp(void){
    OTA.isUpdate = false;
    setUp.isDevCen = false;
    setUp.isDev1 = false;
    setUp.isDev2 = false;
    setUp.isNextion = false;

    if(strcmp(ReadBaseMac(), MAC_CEN) == 0){
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
        if(setUp.isNextion == true){
            // sendData("page 0\xFF\xFF\xFF");
            char* data_out = (char*)malloc(TX_BUF_SIZE);
            sprintf(data_out, "Setting.wifi_id.txt=\"%s\"\xFF\xFF\xFF",WifiData.WifiName);
            sendData(data_out);  
            sprintf(data_out, "Setting.wifi_pass.txt=\"%s\"\xFF\xFF\xFF",WifiData.WifiPass);
            sendData(data_out);
            sendData("restart.j0.val=0\xFF\xFF\xFF");
            sendData("updateProcess.j1.val=0\xFF\xFF\xFF");
            sendData("page 0\xFF\xFF\xFF");
        }
        setUp.isDevCen = true;
    } 
    else if(strcmp(ReadBaseMac(), MAC_1) == 0) setUp.isDev1 = true;
    else if(strcmp(ReadBaseMac(), MAC_2) == 0) setUp.isDev2 = true;
}