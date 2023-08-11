#include "ota.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../INCLUDE/common.h"
#include "../INCLUDE/config.h"
#include "../NEXTION/nextion.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char * TAG = "OTA";

TaskHandle_t OTATaskHandle;

esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

static void OTATask(void* arg)
{
    ESP_LOGI(TAG, "Starting OTATask");
    ESP_LOGI(TAG, "FIRMWARE UPGRADE FWURL: %s", OTA_URL);
    esp_http_client_config_t config = {
        .url = OTA_URL,
        .cert_pem = (char*) certificate_pem_start,
        .timeout_ms = 15000,
        .event_handler = http_event_handler,
        .keep_alive_enable = true,
    };
    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Firmware upgrade success");
        esp_restart();
        // sendData("page 0\xFF\xFF\xFF");
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed");
        esp_restart();
    }
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void OTARun(void)
{
    ESP_LOGI(TAG, "Starting OTA");
    xTaskCreate(&OTATask, "OTATask", 1024*4, NULL, 1, &OTATaskHandle);
}