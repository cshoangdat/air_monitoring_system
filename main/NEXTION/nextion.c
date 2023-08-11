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
#include "../OTA/ota.h"
#include "../SENSOR/sensorRead.h"
#include "../LORA/loradev.h"
#include "../AWS/aws.h"

static const char *TX_TAG               =                      "Nextion_TxTask";
static const char *RX_TAG               =                      "Nextion_RxTask";

TaskHandle_t TxTaskHandle;
TaskHandle_t RxTaskHandle;

static void DelOneChar(char* char_data, const char erase_char)
{
    uint16_t data = strlen(char_data);            
    for(uint16_t i=0; i<data; i++)        
    if(char_data[i] == erase_char) {                       
        for(uint16_t j=i--; j<data;j++)   
            char_data[j]=char_data[j+1];                
        data--;                           
    }
}

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
    setUp.isNextion = true;
}

static void NextionPacket(char* dataOut, char* devName){
    char* devRevData = (char*)malloc(256 * sizeof(char));       
    if(sensorData.dataSensorRev != 0 && sensorData.dataSensorRev != NULL){
        sprintf(devRevData, "%s", sensorData.dataSensorRev);
        ESP_LOGW(TX_TAG,"devRevData: %s", devRevData);
        cJSON *data_receive = cJSON_Parse(devRevData);
        cJSON* dev = cJSON_GetObjectItem(data_receive, devName);
        if(cJSON_GetObjectItem(dev, "temperature")){
            double devTemp = cJSON_GetObjectItem(dev, "temperature")->valuedouble;
            sprintf(dataOut, "%s.temp_%s.txt=\"%.3f\"\xFF\xFF\xFF",devName, devName, devTemp);
            sendData(dataOut);
        }
        if(cJSON_GetObjectItem(dev, "humidity")){
            double devHumid = cJSON_GetObjectItem(dev, "humidity")->valuedouble;
            sprintf(dataOut, "%s.humid_%s.txt=\"%.3f %%\"\xFF\xFF\xFF",devName, devName, devHumid);
            sendData(dataOut);
        }
        if(cJSON_GetObjectItem(dev, "pressure")){
            double devPress = cJSON_GetObjectItem(dev, "pressure")->valuedouble;
            sprintf(dataOut, "%s.press_%s.txt=\"%.3f Pa\"\xFF\xFF\xFF",devName, devName, devPress);
            sendData(dataOut);
        }
        if(cJSON_GetObjectItem(dev, "eCO2")){
            int devCo2 = cJSON_GetObjectItem(dev, "eCO2")->valueint;
            sprintf(dataOut, "%s.co2_%s.txt=\"%d ppm\"\xFF\xFF\xFF",devName, devName, devCo2);
            sendData(dataOut);
        }
        if(cJSON_GetObjectItem(dev, "TVOC")){
            int devTvoc = cJSON_GetObjectItem(dev, "TVOC")->valueint;
            sprintf(dataOut, "%s.tvoc_%s.txt=\"%d ppb\"\xFF\xFF\xFF",devName, devName, devTvoc);
            sendData(dataOut);
        }
        cJSON_Delete(data_receive);
    }
    free(devRevData);
}

static void NextionTxTask(void *arg)
{
    char* dataOut = (char*)malloc(TX_BUF_SIZE);
    while (1) {
        NVSDriverReadU8(NVS_NAMESPACE_PAGE, NVS_KEY_PAGE_2, &pageDev.pageDev0);
        NVSDriverReadU8(NVS_NAMESPACE_DEV, NVS_KEY_DEV_1, &pageDev.pageDev1);
        NVSDriverReadU8(NVS_NAMESPACE_DEV, NVS_KEY_DEV_2, &pageDev.pageDev2);  
        if(pageDev.pageDev0 == 1){
            sprintf(dataOut, "devCenter.temp_dev0.txt=\"%.3f\"\xFF\xFF\xFF",sensorData.shtTemp);
            sendData(dataOut);
            sprintf(dataOut, "devCenter.humid_dev0.txt=\"%.3f %%\"\xFF\xFF\xFF",sensorData.shtHumid);
            sendData(dataOut);
            sprintf(dataOut, "devCenter.press_dev0.txt=\"%.3f Pa\"\xFF\xFF\xFF",sensorData.bmp280Pressure);
            sendData(dataOut);
            sprintf(dataOut, "devCenter.co2_dev0.txt=\"%d ppm\"\xFF\xFF\xFF",sensorData.sgp30Co2);
            sendData(dataOut);
            sprintf(dataOut, "devCenter.tvoc_dev0.txt=\"%d ppb\"\xFF\xFF\xFF",sensorData.sgp30Tvoc);
            sendData(dataOut);
        }
        else if(pageDev.pageDev1 == 1){
            loraSendFLag(DEV_NAME_1);
            if(strcmp(loraRevPass(), "OK") == 0){
                NextionPacket(dataOut, DEV_NAME_1);
            }
        }
        else if(pageDev.pageDev2 == 1){
            NextionPacket(dataOut, DEV_NAME_2);
        }
        if(OTA.isUpdate == true){
            vTaskDelete(TxTaskHandle);
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
        else if(strcmp(value_type_cmd, "D1ON") == 0){
            NVSDriverWriteU8(NVS_NAMESPACE_DEV, NVS_KEY_DEV_1, 1);
        }
        else if(strcmp(value_type_cmd, "D2ON") == 0){
            NVSDriverWriteU8(NVS_NAMESPACE_DEV, NVS_KEY_DEV_2, 1);
        }
        else if(strcmp(value_type_cmd, "UD") == 0){
            OTA.isUpdate = true;
            ESP_LOGW(RX_TAG, "UPDATE");
            OTARun();
            char* data_out = (char*)malloc(TX_BUF_SIZE);
            for(int i = 1; i<= 100; i++){
                sprintf(data_out, "updateProcess.j1.val=%d\xFF\xFF\xFF",i);
                sendData(data_out);
                vTaskDelay(500/portTICK_PERIOD_MS);
            }
            vTaskDelete(RxTaskHandle);
        }
        else if(strcmp(value_type_cmd, "WN") == 0){
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
            // sendData("page 0\xFF\xFF\xFF");
        }
        else{
            NVSDriverWriteU8(NVS_NAMESPACE_PAGE, NVS_KEY_PAGE_2, 0);
            NVSDriverWriteU8(NVS_NAMESPACE_DEV, NVS_KEY_DEV_1, 0);
            NVSDriverWriteU8(NVS_NAMESPACE_DEV, NVS_KEY_DEV_2, 0);
        }
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
  free(data);
  free(dstream);
}

void NextionRun(void){
    xTaskCreate(NextionTxTask, "NextionTxTask", 1024*6, NULL, 1, TxTaskHandle);
    xTaskCreate(NextionRxTask, "NextionRxTask", 1024*5, NULL, 2, RxTaskHandle);
}

void NextionStop(void){
    vTaskDelete(RxTaskHandle);
    vTaskDelete(TxTaskHandle);
}