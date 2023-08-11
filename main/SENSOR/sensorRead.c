#include "sensorRead.h"
#include "../I2C/i2cDriver.h"
#include "sht4x.h"
#include "bmp280.h"
#include "../SENSOR/sgp30Read.h"
#include "../SENSOR/sgp30Driver.h"
#include "../INCLUDE/common.h"
#include "../INCLUDE/config.h"
#include "esp_log.h"

static const char *TAG              =                   "SensorRead";

TaskHandle_t BMETaskHandle;
TaskHandle_t SGPTaskHandle;
TaskHandle_t SHTTaskHandle;


static void SHT4xTask(void *pvParameter){
    static sht4x_t sht4x_dev;
    if(setUp.isI2cInit == false){
        ESP_ERROR_CHECK(i2cdev_init());
        ESP_LOGI(TAG, "I2C init complete");
        setUp.isI2cInit = true;
    }
    if(setUp.isSht4xInit == false){
        memset(&sht4x_dev, 0, sizeof(sht4x_t));
        ESP_ERROR_CHECK(sht4x_init_desc(&sht4x_dev, 0, I2C_MASTER_SDA_PIN, I2C_MASTER_SCL_PIN));
        ESP_ERROR_CHECK(sht4x_init(&sht4x_dev));
        setUp.isSht4xInit = true;
    }
    while (1){
        ESP_ERROR_CHECK(sht4x_measure(&sht4x_dev, &sensorData.shtTemp, &sensorData.shtHumid));
        ESP_LOGI(TAG,"sht4x Sensor: %.2f °C, %.2f %%", sensorData.shtTemp, sensorData.shtHumid);
        if(OTA.isUpdate == true){vTaskDelete(SHTTaskHandle);}
        vTaskDelay(1000/portTICK_PERIOD_MS);
        // printf("remaining memory of task Read SHT4x : %d byte",uxTaskGetStackHighWaterMark(NULL));
    }
}

static void BMP280Task(void *pvParameter){
    bmp280_params_t params;
    bmp280_t dev;
    if(setUp.isI2cInit == false){
        ESP_ERROR_CHECK(i2cdev_init());
        ESP_LOGI(TAG, "I2C init complete");
        setUp.isI2cInit = true;
    }
    if(setUp.isBmp280Init == false){
        bmp280_init_default_params(&params);
        memset(&dev, 0, sizeof(bmp280_t));
        ESP_ERROR_CHECK(bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_0, 0, I2C_MASTER_SDA_PIN, I2C_MASTER_SCL_PIN));
        ESP_ERROR_CHECK(bmp280_init(&dev, &params));
        setUp.isBmp280Init = true;
    }
    while(1){
        if (bmp280_read_float(&dev, &sensorData.bmp280Temp, &sensorData.bmp280Pressure, &sensorData.bmp280Humid) != ESP_OK)
        {
            printf("Temperature/pressure reading failed\n");
            continue;
        }
        // ?
        // sensorData.bmp280Humid = 73 + (float)( rand() * ((int)((2.5 - (-2.5)) / 0.243) + 1) * 0.243 / RAND_MAX );
        ESP_LOGI(TAG,"BMP280 Sensor: Pressure: %.2f Pa", sensorData.bmp280Pressure);
        if(OTA.isUpdate == true){vTaskDelete(BMETaskHandle);}
        vTaskDelay(1000/portTICK_PERIOD_MS);
        // printf("remaining memory of task Read BMP280 : %d byte",uxTaskGetStackHighWaterMark(NULL));
    }
}

static void SGP30Task(void *pvParameter){
    sgp30_dev_t main_sgp30_sensor;
    if(setUp.isI2cInit == false){
        ESP_ERROR_CHECK(i2c_master_init());
        ESP_LOGI(TAG, "I2C init complete");
        setUp.isI2cInit = true;
    } 
    if(setUp.isSgp30Init == false){
        sgp30_init(&main_sgp30_sensor, (sgp30_read_fptr_t)main_i2c_read, (sgp30_write_fptr_t)main_i2c_write);
        for (uint8_t i = 0; i < 14; i++) {
            vTaskDelay(500/portTICK_PERIOD_MS);
            sgp30_IAQ_measure(&main_sgp30_sensor);
            ESP_LOGI(TAG, "SGP30 Calibrating... TVOC: %d,  eCO2: %d",  main_sgp30_sensor.TVOC, main_sgp30_sensor.eCO2);
            uint16_t eco2_baseline, tvoc_baseline;
            sgp30_get_IAQ_baseline(&main_sgp30_sensor, &eco2_baseline, &tvoc_baseline);
            ESP_LOGI(TAG, "BASELINES - TVOC: %d,  eCO2: %d",  tvoc_baseline, eco2_baseline);
        }
        setUp.isSgp30Init = true;
    }
    while(1) {
        sgp30_IAQ_measure(&main_sgp30_sensor);
        sensorData.sgp30Co2 = main_sgp30_sensor.eCO2;
        sensorData.sgp30Tvoc = main_sgp30_sensor.TVOC;
        ESP_LOGI(TAG, "TVOC: %d,  eCO2: %d",  sensorData.sgp30Tvoc, sensorData.sgp30Co2);
        // printf("remaining memory of task_read_sgp30 : %d byte",uxTaskGetStackHighWaterMark(NULL));
        if(OTA.isUpdate == true){vTaskDelete(SGPTaskHandle);}
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void SensorRead(void){
    xTaskCreate(BMP280Task, "BMP280Task", 1024*2, NULL, 9, BMETaskHandle);
    xTaskCreate(SHT4xTask,  "SHT4xTask",  1024*2, NULL, 9, SHTTaskHandle);
    xTaskCreate(SGP30Task,  "SGP30Task",  1024*2, NULL, 9, SGPTaskHandle);
}