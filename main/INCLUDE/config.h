#ifndef __CONFIG__
#define __CONFIG__

#include "driver/i2c.h"

#define UART_TXD_PIN                                           (3)
#define UART_RXD_PIN                                           (10)
#define UART_PORT_NUM                                          UART_NUM_1
#define UART_BAUD_RATE                                         (115200)
#define TX_BUF_SIZE                                            (256)
#define RX_BUF_SIZE                                            (1024) 

#define I2C_MASTER_SDA_PIN                                     (19)
#define I2C_MASTER_SCL_PIN                                     (18)
#define I2C_MASTER_NUM                                         (0)
#define I2C_MASTER_FREQ_HZ                                     (400000)
#define I2C_MASTER_TX_BUF_DISABLE                              (0)
#define I2C_MASTER_RX_BUF_DISABLE                              (0)
#define I2C_MASTER_TIMEOUT                                     (1000)
#define ACK_VAL                                                (0x0)
#define NACK_VAL                                               (0x1)
#define ACK_CHECK_EN                                           (0x1)
#define ACK_CHECK_DIS                                          (0x0)
#define I2C_WRITE_BIT                                          I2C_MASTER_WRITE
#define I2C_READ_BIT                                           I2C_MASTER_READ

#define MOSI_PIN                                                GPIO_NUM_10
#define SPI_CLOCK_SPEED_HZ                                      4000000

#define NVS_NAMESPACE_CONFIG                                    "CONFIG"
#define NVS_KEY_WIFI_SSID                                       "WSSID"
#define NVS_KEY_WIFI_PASS                                       "WPASS"
#define NVS_NAMESPACE_PAGE                                      "PAGE"
#define NVS_KEY_PAGE_2                                          "PAGE2"

#define DEFAULT_SSID                                            "hihi"
#define DEFAULT_PASS                                            "123456890"
#define AWS_URI                                                 "mqtts://a3suuuxay09k3c-ats.iot.us-east-2.amazonaws.com"
#define ESP_SOFT_RESET                                          "espreset"


#endif