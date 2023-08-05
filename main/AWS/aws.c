#include "aws.h"
#include "esp_log.h"
#include <stdint.h>
#include <stddef.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "mqtt_client.h"
#include "esp_tls.h"
#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"
#include "cJSON.h"

#include "INCLUDE/common.h"
#include "INCLUDE/config.h"

static const char* TAG              =                 "AWS";
static const char* TOPIC            =                 "Data";
// static const char* TOPIC_RECIEVE    =                 "recieve";
TaskHandle_t AWSTaskHandle;
static char mac_str[18];
static const char* base_mac                =                 "10:91:A8:01:5D:E0";

esp_mqtt_client_handle_t client;
esp_mqtt_event_handle_t event;

static void MQTT_Handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    event = event_data;
    client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, TOPIC, 0);
        // esp_mqtt_client_subscribe(client, TOPIC_RECIEVE, 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)",  event->error_handle->esp_transport_sock_errno,
            strerror(event->error_handle->esp_transport_sock_errno));
        } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        } else {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void MQTT_Start(const char* URI)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = URI,
        .port = 8883,
        .cert_pem = (const char*) cert_start,
        .client_cert_pem = (const char*) certificate_start,
        .client_key_pem = (const char*) private_start,
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, MQTT_Handler, NULL);
    esp_mqtt_client_start(client);
}

static void MQTT_Stop(void){
    esp_mqtt_client_disconnect(client);
    vTaskDelay(500/portTICK_PERIOD_MS);
    esp_mqtt_client_stop(client);
    vTaskDelay(500/portTICK_PERIOD_MS);
    esp_mqtt_client_destroy(client);
}

static void ReadBaseMac(void){
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
    }
}

static void AWS_Task(void *arg){
    bool is_dev_center = false;
    cJSON* sensor_data, *dev0;
    static char* pub_sensor_data = NULL;
    ReadBaseMac();
    if(strcmp(mac_str, base_mac) == 0) is_dev_center = true;
    MQTT_Start(AWS_URI);
    while(1){
        if(is_dev_center == true){
            if(sensorData.shtHumid != 0 && sensorData.shtTemp != 0 && sensorData.bmp280Pressure != 0 && sensorData.sgp30Co2 != 0 && sensorData.sgp30Tvoc != 0){
                dev0 = cJSON_CreateObject();
                cJSON_AddItemToObject(dev0, "dev0", sensor_data = cJSON_CreateObject());
                cJSON_AddNumberToObject(sensor_data, "temperature", sensorData.shtTemp);
                cJSON_AddNumberToObject(sensor_data, "humidity", sensorData.shtHumid);
                cJSON_AddNumberToObject(sensor_data, "pressure", sensorData.bmp280Pressure);
                cJSON_AddNumberToObject(sensor_data, "eCO2", sensorData.sgp30Co2);
                cJSON_AddNumberToObject(sensor_data, "TVOC", sensorData.sgp30Tvoc);
                pub_sensor_data = cJSON_Print(dev0);
                int msg_id = esp_mqtt_client_publish(client, TOPIC, pub_sensor_data, 0, 0, 0);
                ESP_LOGI(TAG,"sent publish Data successful, msg_id=%d", msg_id);
                ESP_LOGI(TAG,"dev0:%s", pub_sensor_data);
                cJSON_free(pub_sensor_data);
                cJSON_Delete(dev0);
            }
        }
        if(setUp.isLoraRev == true){
           int msg_id = esp_mqtt_client_publish(client, TOPIC, (char*)sensorData.dataSensorRev, 0, 0, 0);
           ESP_LOGI(TAG,"sent publish Data successful, msg_id=%d", msg_id);
           ESP_LOGI(TAG,"Data Sensor:%s", sensorData.dataSensorRev);
        }
        vTaskDelay(500/portTICK_PERIOD_MS);
    // printf("remaining memory of task_transmit_to_AWS : %d byte",uxTaskGetStackHighWaterMark(NULL));
    }  
}

void AWS_Run(void){
    xTaskCreate(&AWS_Task, "AWS_Task", 1024*2, NULL, 4,  AWSTaskHandle);
}

void AWS_Stop(void){
    vTaskDelete(AWSTaskHandle);
    AWSTaskHandle = NULL;
    esp_mqtt_client_stop(client);
}