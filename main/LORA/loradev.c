#include "loradev.h"
#include "lora.h"
#include "cJSON.h"
#include "INCLUDE/common.h"
#include "INCLUDE/config.h"
#include "string.h"
#include "esp_log.h"

const char* TAG = "loradev";
TaskHandle_t loraTaskRevHandle;
TaskHandle_t loraTaskTransHandle;

void LoraInit(void){
    if (lora_init() == 0) {
        ESP_LOGE(TAG, "Does not recognize the module");
        while(1) {
            vTaskDelay(1);
        }
    }
    ESP_LOGI(TAG, "Frequency is 433MHz");
    lora_set_frequency(433e6); // 433MHz
    lora_enable_crc();
    int cr = 1;
    int bw = 9;
    int sf = 12;
    lora_set_coding_rate(cr);
    ESP_LOGI(TAG, "coding_rate=%d", cr);

    lora_set_bandwidth(bw);
    ESP_LOGI(TAG, "bandwidth=%d", bw);

    lora_set_spreading_factor(sf);
    ESP_LOGI(TAG, "spreading_factor=%d", sf);
    ESP_LOGI(TAG, "Start");
}

static void loraSendPacket(char* devName){
    cJSON* sensor_data, *dev;
    static char* pub_sensor_data = NULL;
    if(sensorData.shtTemp != 0 && sensorData.shtHumid != 0 && sensorData.bmp280Pressure != 0 && sensorData.sgp30Co2 != 0){
        dev = cJSON_CreateObject();
        cJSON_AddItemToObject(dev, devName, sensor_data = cJSON_CreateObject());
        cJSON_AddNumberToObject(sensor_data, "temperature", sensorData.shtTemp);
        cJSON_AddNumberToObject(sensor_data, "humidity", sensorData.shtHumid);
        cJSON_AddNumberToObject(sensor_data, "pressure", sensorData.bmp280Pressure);
        cJSON_AddNumberToObject(sensor_data, "eCO2", sensorData.sgp30Co2);
        cJSON_AddNumberToObject(sensor_data, "TVOC", sensorData.sgp30Tvoc);
        pub_sensor_data = cJSON_Print(dev);
        lora_send_packet((uint8_t*)pub_sensor_data, strlen(pub_sensor_data));
        ESP_LOGI(TAG,"%s", pub_sensor_data);
        ESP_LOGI(TAG, "%d byte packet sent...", sizeof(pub_sensor_data));
        cJSON_free(pub_sensor_data);
        cJSON_Delete(dev);
    }
}

void loraSendFLag(char*devName){
    lora_send_packet((uint8_t*)devName, strlen(devName));
    ESP_LOGI(TAG,"send flag to %s", devName);
}

bool loraRevPass(){
    uint8_t flag[3];
    bool isLoraRevPass = false;
    while(1) {
		lora_receive();
		if (lora_received()) {
            memset(flag, 0 , sizeof(flag));
			int receive_len = lora_receive_packet(flag, sizeof(flag));
			ESP_LOGI(TAG, "%d byte packet received:%.*s", receive_len, receive_len, flag);
            if(strcmp((char*) flag, "OK") == 0){
            }
		}
        vTaskDelay(1);
    }
}

char* loraRevFlag(char*devName){
    uint8_t flag[strlen(devName)];
    while(1) {
		lora_receive();
		if (lora_received()) {
            memset(flag, 0 , sizeof(flag));
			int receive_len = lora_receive_packet(flag, sizeof(flag));
			ESP_LOGI(TAG, "%d byte packet received:%.*s", receive_len, receive_len, flag);
            return (char*)flag;
		}
        vTaskDelay(1);
    }
}

static void LoraRevTask(void *pvParameter){
    setUp.isLoraRev = false;
	while(1) {
		lora_receive();
		if (lora_received()) {
            setUp.isLoraRev = true;
            memset(sensorData.dataSensorRev, 0 , sizeof(sensorData.dataSensorRev));
			int receive_len = lora_receive_packet(sensorData.dataSensorRev, sizeof(sensorData.dataSensorRev));
			ESP_LOGI(TAG, "%d byte packet received:%.*s", receive_len, receive_len, sensorData.dataSensorRev);
		}
        if(OTA.isUpdate == true){
            vTaskDelete(loraTaskRevHandle);
        }
		// vTaskDelay(100/portTICK_PERIOD_MS);
        vTaskDelay(1);
    // printf("remaining memory of task_lora_rev : %d byte",uxTaskGetStackHighWaterMark(NULL));
	}
}

static void LoraTransTask(void *pvParameter){
	while(1) {
        if(setUp.isDev1 == true){
            loraSendPacket(DEV_NAME_1);
            vTaskDelay(5500/portTICK_PERIOD_MS);
        }
        if(setUp.isDev2 == true){
            loraSendPacket(DEV_NAME_2);
            vTaskDelay(5000/portTICK_PERIOD_MS);
        }
        if(OTA.isUpdate == true){
            vTaskDelete(loraTaskTransHandle);
        }
        // int lost = lora_packet_lost();
		// if (lost != 0) {
		// 	ESP_LOGW(TAG, "%d packets lost", lost);
		// }
        // printf("remaining memory of task_lora_trans : %d byte",uxTaskGetStackHighWaterMark(NULL));
	}
}

void LoraRev(void){
    xTaskCreate(LoraRevTask,  "LoraRevTask",  1024*4, NULL, 5, loraTaskRevHandle);
}

void LoraTrans(void){
    xTaskCreate(LoraTransTask,  "LoraTransTask",  1024*4, NULL, 5, loraTaskTransHandle);
}

void LoraStop(void){
    vTaskDelete(loraTaskRevHandle);
    vTaskDelete(loraTaskTransHandle);
}