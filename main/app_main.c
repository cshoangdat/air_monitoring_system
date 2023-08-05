#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "cJSON.h"
#include "esp_netif.h"
#include "esp_event.h"

#include "../WS2812B/ws2812b.h"
#include "../AWS/aws.h"
#include "../WIFI/WiFiDriver.h"
#include "../NVS/NVSDriver.h"
#include "../INCLUDE/common.h"
#include "../INCLUDE/config.h"
#include "../SENSOR/sensorRead.h"
#include "../LORA/loradev.h"
#include "../NEXTION/nextion.h"
#include "../BASIC/appTask.h"

#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Booting...");
    ESP_LOGI(TAG, "Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "IDF version: %s", esp_get_idf_version());
    appSetUp();
    SensorRead();
    NextionRun();
    esp_err_t err = WiFiDriverConnect(WifiData.WifiName, WifiData.WifiPass);
    if(err != ESP_OK){
        ESP_LOGE(TAG, "WiFi Driver Connect Failed");
    }
    LoraRev();
    AWS_Run();
}
