#include "i2cDriver.h"
#include <stdio.h>
#include "driver/i2c.h"
#include "esp_console.h"
#include "esp_log.h"

#include "INCLUDE/common.h"
#include "INCLUDE/config.h"

static uint8_t i2c_master_port =                        I2C_MASTER_NUM;
static const char* TAG =                                "i2cdev";

esp_err_t i2c_master_init(void){
    static const i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_PIN,
        .scl_io_num = I2C_MASTER_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed  = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void i2c_detect(void)
{
     printf("i2c scan: \n");
     for (uint8_t i = 1; i < 127; i++)
     {
        int ret;
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, 1);
        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);
    
        if (ret == ESP_OK)
        {
            printf("Found device at: 0x%2x\n", i);
        }
    }
}