#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define WIFI_SSID      "Wilfred"
#define WIFI_PASS      "Lalalala"

static const char *TAG = "wifi_wpa3";

static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected. Reconnecting...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

void wifi_init_sta(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
    .sta = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
        .threshold.authmode = WIFI_AUTH_WPA2_WPA3_PSK
     }
        };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");
}
void wifi_status_task(void *pvParameter) {
    wifi_ap_record_t ap_info;

    while (1) {
        if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
            switch (ap_info.authmode) {
                case WIFI_AUTH_OPEN:
                    ESP_LOGI(TAG, "Connected: %s | Security: Open", (char*)ap_info.ssid);
                    break;
                case WIFI_AUTH_WEP:
                    ESP_LOGI(TAG, "Connected: %s | Security: WEP", (char*)ap_info.ssid);
                    break;
                case WIFI_AUTH_WPA2_PSK:
                    ESP_LOGI(TAG, "Connected: %s | Security: WPA2-Personal", (char*)ap_info.ssid);
                    break;
                case WIFI_AUTH_WPA3_PSK:
                    ESP_LOGI(TAG, "Connected: %s | Security: WPA3-Personal", (char*)ap_info.ssid);
                    break;
                case WIFI_AUTH_WPA2_WPA3_PSK:
                    ESP_LOGI(TAG, "Connected: %s | Security: WPA2/WPA3 Transition", (char*)ap_info.ssid);
                    break;
                default:
                    ESP_LOGI(TAG, "Connected: %s | Security: Other", (char*)ap_info.ssid);
                    break;
            }
        } else {
            ESP_LOGW(TAG, "Not connected to any Wi-Fi");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));  // wait 2 seconds
    }
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    xTaskCreate(&wifi_status_task, "wifi_status_task", 4096, NULL, 5, NULL);
    wifi_init_sta();
}
