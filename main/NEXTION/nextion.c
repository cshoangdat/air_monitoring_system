#include "driver/gpio.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "nextion.h"
#include "INCLUDE/common.h"
#include "INCLUDE/config.h"
#include "string.h"
#include "esp_log.h"
#include "cJSON.h"
#include "../NVS/NVSDriver.h"

static const char *TX_TAG               =                      "Nextion_TxTask";
static const char *RX_TAG               =                      "Nextion_RxTask";

TaskHandle_t TxTaskHandle;
TaskHandle_t RxTaskHandle;

int sendData(const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_PORT_NUM, data, len);
    ESP_LOGI(TX_TAG, "Wrote %d bytes", txBytes);
    return txBytes;
}

void initNextion() {
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TXD_PIN, UART_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

static void NextionTxTask(void *arg)
{
    char* data_out = (char*)malloc(TX_BUF_SIZE);
    while (1) {
        NVSDriverReadU8(NVS_NAMESPACE_PAGE, NVS_KEY_PAGE_2, &pageDev.pageDev0);  
        if(pageDev.pageDev0 == 1){
            sprintf(data_out, "devCenter.temp_dev0.txt=\"%.3f\"\xFF\xFF\xFF",sensorData.shtTemp);
            sendData(data_out);
            sprintf(data_out, "devCenter.humid_dev0.txt=\"%.3f %%\"\xFF\xFF\xFF",sensorData.shtHumid);
            sendData(data_out);
            sprintf(data_out, "devCenter.press_dev0.txt=\"%.3f Pa\"\xFF\xFF\xFF",sensorData.bmp280Pressure);
            sendData(data_out);
            sprintf(data_out, "devCenter.co2_dev0.txt=\"%d ppm\"\xFF\xFF\xFF",sensorData.sgp30Co2);
            sendData(data_out);
            sprintf(data_out, "devCenter.tvoc_dev0.txt=\"%d ppb\"\xFF\xFF\xFF",sensorData.sgp30Tvoc);
            sendData(data_out);
        }
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

static void NextionRxTask(void *arg)
{
  ESP_LOGD(RX_TAG,"NEXTION RX TASK");
  uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
  char *dstream = malloc(RX_BUF_SIZE+1);
  while (1) {
    ESP_LOGD(RX_TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    memset(dstream,0,sizeof(malloc(RX_BUF_SIZE+1)));
    const int rxBytes = uart_read_bytes(UART_PORT_NUM, data, RX_BUF_SIZE, 100/portTICK_RATE_MS);
    if (rxBytes > 0) {
      data[rxBytes] = '\0';
      snprintf(dstream, RX_BUF_SIZE+1, "%s", data);
      ESP_LOGI(RX_TAG, "rxBytes:%d", rxBytes);
    }
    ESP_LOGI(RX_TAG, "data recv %s", dstream);

    cJSON *sub;
    sub=cJSON_Parse(dstream);

    cJSON *body =cJSON_GetObjectItem(sub,"body");
    char *value_body;
    //char *value_type_cmd;
    if(body){
        value_body=cJSON_GetObjectItem(sub,"body")->valuestring;
        ESP_LOGW(RX_TAG, "body is %s",value_body);
    }else{
        value_body="";
    }
    cJSON *command =cJSON_GetObjectItem(sub,"command");
    if(command){
        char *value_type_cmd =cJSON_GetObjectItem(sub,"command")->valuestring;
        ESP_LOGW(RX_TAG, "command is %s",value_type_cmd);
        if(strcmp(value_type_cmd, "D0") == 0){
            NVSDriverWriteU8(NVS_NAMESPACE_PAGE, NVS_KEY_PAGE_2, 1);
        }
        else{
            NVSDriverWriteU8(NVS_NAMESPACE_PAGE, NVS_KEY_PAGE_2, 0);
        }
        if(strcmp(value_type_cmd, "WN") == 0){
            ESP_LOGW(RX_TAG, "Save Wifi SSID = %s", value_body);
            NVSDriverWriteString(NVS_NAMESPACE_CONFIG, NVS_KEY_WIFI_SSID, value_body);
        }
        else if(strcmp(value_type_cmd, "WP") == 0){
            ESP_LOGW(RX_TAG, "Save Wifi PASS = %s", value_body);
            NVSDriverWriteString(NVS_NAMESPACE_CONFIG, NVS_KEY_WIFI_PASS, value_body);           
        }
        else if(strcmp(value_type_cmd, "RST") == 0){
            ESP_LOGW(RX_TAG, "RESET");
            char* data_out = (char*)malloc(TX_BUF_SIZE);
            for(int i = 1; i<= 100; i++){
                sprintf(data_out, "restart.j0.val=%d\xFF\xFF\xFF",i);
                sendData(data_out);
                vTaskDelay(10/portTICK_PERIOD_MS);
            }
            esp_restart();
        }
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
  free(data);
  free(dstream);
}

void NextionRun(void){
    xTaskCreate(NextionTxTask, "NextionTxTask", 1024*6, NULL, 4, TxTaskHandle);
    xTaskCreate(NextionRxTask, "NextionRxTask", 1024*5, NULL, 6, RxTaskHandle);
}

void NextionStop(void){
    vTaskDelete(TxTaskHandle);
    vTaskDelete(RxTaskHandle);
    TxTaskHandle = NULL;
    RxTaskHandle = NULL;
    uart_driver_delete(UART_PORT_NUM);
}