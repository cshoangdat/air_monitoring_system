#include "ws2812B.h"
#include "esp_log.h"

#include "INCLUDE/common.h"
#include "INCLUDE/config.h"

#define TAG "SPI"

static SPI_setting_t SPI_setting = {
    .host = SPI2_HOST,
    .buscfg = {
        .mosi_io_num = MOSI_PIN,
        .max_transfer_sz = LED_DMA_BUFFER_SIZE,
        .miso_io_num = -1,
        .sclk_io_num = -1,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
    },
    .devcfg = {
        .mode = 0,
        .clock_speed_hz = SPI_CLOCK_SPEED_HZ,
        .spics_io_num = -1,
        .queue_size = 1,
        .command_bits = 0,
        .address_bits = 0
    }
};

void init_spi(void){
    esp_err_t ret;
    ret = spi_bus_initialize(SPI_setting.host, &SPI_setting.buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    ret = spi_bus_add_device(SPI_setting.host, &SPI_setting.devcfg, &SPI_setting.spi);
    ESP_ERROR_CHECK(ret);
    led_DMA_buffer = heap_caps_malloc(LED_DMA_BUFFER_SIZE, MALLOC_CAP_DMA);

    int khz;
    khz = spi_get_actual_clock(APB_CLK_FREQ, SPI_CLOCK_SPEED_HZ, 128);
    ESP_LOGI(TAG, "spi_get_actual_clock %d", khz);
    ESP_LOGI(TAG, "Complete Inittializing SPI");
}

void update_led_strip(uint32_t* bufLed){
    uint16_t LedBitPattern[16] = {
    0x8888,
    0x8C88,
    0xC888,
    0xCC88,
    0x888C,
    0x8C8C,
    0xC88C,
    0xCC8C,
    0x88C8,
    0x8CC8,
    0xC8C8,
    0xCCC8,
    0x88CC,
    0x8CCC,
    0xC8CC,
    0xCCCC
    };
    memset(led_DMA_buffer,0,LED_DMA_BUFFER_SIZE);
    int n = 0;
    uint32_t temp = bufLed[0];

    // R
    led_DMA_buffer[n++] = LedBitPattern[0x0f & (temp >> 12)];
    led_DMA_buffer[n++] = LedBitPattern[0x0f & (temp >> 8)];

    // G
    led_DMA_buffer[n++] = LedBitPattern[0x0f & (temp >> 4)];
    led_DMA_buffer[n++] = LedBitPattern[0x0f & temp];

    // B
    led_DMA_buffer[n++] = LedBitPattern[0x0f & (temp >> 20)];
    led_DMA_buffer[n++] = LedBitPattern[0x0f & (temp >> 16)];

    spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length = LED_DMA_BUFFER_SIZE * 8; //length is in bits
	t.tx_buffer = led_DMA_buffer;

	esp_err_t err = spi_device_transmit(SPI_setting.spi, &t);
	ESP_ERROR_CHECK(err);
}


