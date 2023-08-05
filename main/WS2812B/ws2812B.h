#ifndef WS2812B
#define WS2812B

#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define LED_DMA_BUFFER_SIZE             (16*12)+1

typedef struct{
    spi_host_device_t host;
    spi_device_handle_t spi;
    spi_device_interface_config_t devcfg;
    spi_bus_config_t buscfg;
} SPI_setting_t;

typedef struct CRGB {
        uint8_t r;
        uint8_t g;
        uint8_t b;
}CRGB;

uint16_t* led_DMA_buffer;

void init_spi(void);
void update_led_strip(uint32_t* bufLed);

#endif