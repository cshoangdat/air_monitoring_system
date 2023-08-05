#include "WiFiDriver.h"
#include "INCLUDE/common.h"
#include "INCLUDE/config.h"
#include "../NEXTION/nextion.h"

#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#define WIFI_CONNECTED_BIT              BIT0
#define WIFI_FAIL_BIT                   BIT1

static const char *TAG = "STI-IoT WiFi Driver";

static EventGroupHandle_t WiFiEventGroup;
static esp_ip4_addr_t IP4Address;
static esp_netif_t *NetIF = NULL;
static const char* ConnectionName;

static void on_got_ip(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Got IP event!");
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    memcpy(&IP4Address, &event->ip_info.ip, sizeof(IP4Address));
    xEventGroupSetBits(WiFiEventGroup, WIFI_CONNECTED_BIT);
    sendData("devCenter.wifi.txt=\"connect\"\xFF\xFF\xFF");
    sendData("devManage.wifi.txt=\"connect\"\xFF\xFF\xFF");
    sendData("dev1.wifi.txt=\"connect\"\xFF\xFF\xFF");
    sendData("dev2.wifi.txt=\"connect\"\xFF\xFF\xFF");
    sendData("dev3.wifi.txt=\"connect\"\xFF\xFF\xFF");
}
static void on_wifi_disconnect(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
    sendData("devCenter.wifi.txt=\"disconnect\"\xFF\xFF\xFF");
    sendData("devManage.wifi.txt=\"disconnect\"\xFF\xFF\xFF");
    sendData("dev1.wifi.txt=\"disconnect\"\xFF\xFF\xFF");
    sendData("dev2.wifi.txt=\"disconnect\"\xFF\xFF\xFF");
    sendData("dev3.wifi.txt=\"disconnect\"\xFF\xFF\xFF");
    esp_err_t err = esp_wifi_connect();
    if (err == ESP_ERR_WIFI_NOT_STARTED) {
        return;
    }
    ESP_ERROR_CHECK(err);
}
static void WiFiDriverStart(const char* ssid, const char* passwd)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_netif_config_t netif_config = ESP_NETIF_DEFAULT_WIFI_STA();

    esp_netif_t *netif = esp_netif_new(&netif_config);

    assert(netif);

    esp_netif_attach_wifi_station(netif);
    esp_wifi_set_default_wifi_sta_handlers();

    NetIF = netif;

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL));
    // ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config));
    if (ssid) {
        strncpy((char *)wifi_config.sta.ssid, ssid, strlen(ssid));
    }
    if (passwd) {
        strncpy((char *)wifi_config.sta.password, passwd, strlen(passwd));
    }

    ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_connect();
    ConnectionName = ssid;
}
static void WiFiDriverStop(void)
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect));
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip));
    esp_err_t err = esp_wifi_stop();
    if (err == ESP_ERR_WIFI_NOT_INIT) {
        return;
    }
    ESP_ERROR_CHECK(err);
    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(NetIF));
    esp_netif_destroy(NetIF);
    NetIF = NULL;
}
esp_err_t WiFiDriverConnect(const char* ssid, const char* passwd)
{
    if (WiFiEventGroup != NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    WiFiEventGroup = xEventGroupCreate();
    WiFiDriverStart(ssid, passwd);
    ESP_ERROR_CHECK(esp_register_shutdown_handler(&WiFiDriverStop));
    ESP_LOGI(TAG, "Waiting for IP");
    xEventGroupWaitBits(WiFiEventGroup, WIFI_CONNECTED_BIT, true, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to %s", ConnectionName);
    ESP_LOGI(TAG, "IPv4 address: " IPSTR, IP2STR(&IP4Address));
    sprintf(WifiData.DevIP, IPSTR, IP2STR(&IP4Address));
    ESP_LOGI(TAG, "IPv4 address: %s", WifiData.DevIP);
    return ESP_OK;
}
esp_err_t WiFiDriverDisconnect(void)
{
    sendData("devCenter.wifi.txt=\"disconnect\"\xFF\xFF\xFF");
    sendData("devManage.wifi.txt=\"disconnect\"\xFF\xFF\xFF");
    sendData("dev1.wifi.txt=\"disconnect\"\xFF\xFF\xFF");
    sendData("dev2.wifi.txt=\"disconnect\"\xFF\xFF\xFF");
    sendData("dev3.wifi.txt=\"disconnect\"\xFF\xFF\xFF");
    if (WiFiEventGroup == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    vEventGroupDelete(WiFiEventGroup);
    WiFiEventGroup = NULL;
    WiFiDriverStop();
    ESP_LOGI(TAG, "Disconnected from %s", ConnectionName);
    ConnectionName = NULL;
    return ESP_OK;
}