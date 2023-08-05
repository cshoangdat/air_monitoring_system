#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t WiFiDriverConnect(const char* ssid, const char* passwd);
esp_err_t WiFiDriverDisconnect(void);

#ifdef __cplusplus
}
#endif

#endif