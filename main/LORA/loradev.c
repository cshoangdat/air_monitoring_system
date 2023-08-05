#include "loradev.h"
#include "lora.h"
#include "cJSON.h"
#include "INCLUDE/common.h"
#include "INCLUDE/config.h"
#include "string.h"
#include "esp_log.h"

const char* TAG = "loradev";
TaskHandle_t loraTaskHandle;

static void LoraInit(void){
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
    int bw = 7;
    int sf = 7;
    lora_set_coding_rate(cr);
    ESP_LOGI(TAG, "coding_rate=%d", cr);

    lora_set_bandwidth(bw);
    ESP_LOGI(TAG, "bandwidth=%d", bw);

    lora_set_spreading_factor(sf);
    ESP_LOGI(TAG, "spreading_factor=%d", sf);
    ESP_LOGI(TAG, "Start");
}

static void LoraTask(void *pvParameter){
    setUp.isLoraRev = false;
    LoraInit();
	while(1) {
		lora_receive();
		if (lora_received()) {
            setUp.isLoraRev = true;
            memset(sensorData.dataSensorRev, 0 , sizeof(sensorData.dataSensorRev));
			int receive_len = lora_receive_packet(sensorData.dataSensorRev, sizeof(sensorData.dataSensorRev));
			ESP_LOGI(TAG, "%d byte packet received:%.*s", receive_len, receive_len, sensorData.dataSensorRev);
		}
		vTaskDelay(500/portTICK_PERIOD_MS);
    // printf("remaining memory of task_lora_rev : %d byte",uxTaskGetStackHighWaterMark(NULL));
	}
}

void LoraRev(void){
    xTaskCreate(LoraTask,  "LoraTask",  1024*2, NULL, 4, loraTaskHandle);
}

void LoraStop(void){
    lora_close();
    vTaskDelete(loraTaskHandle);
}