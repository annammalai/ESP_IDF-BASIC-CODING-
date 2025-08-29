main.c

Explanation:

WIFI_AUTH_WPA3_PSK → Only WPA3-Personal.

WIFI_AUTH_WPA2_PSK → Accepts WPA2 and also WPA3 if the AP supports transition mode.

WIFI_AUTH_WPA2_WPA3_PSK → Best choice if you want WPA2 or WPA3.

WIFI_AUTH_OPEN → Will connect to any network type (Open, WPA2, WPA3, etc.) without restrictions.

Code Explained (Wi-Fi WPA3 Station)
1. Logging Tag
static const char *TAG = "wifi_wpa3";


TAG is just a name you give to your log messages.

When you use ESP_LOGI(TAG, "..."), ESP-IDF prints like:

I (123) wifi_wpa3: Got IP: 192.168.1.5

2. Event Handler
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)


This is like a callback function.
ESP-IDF sends events (like Wi-Fi started, disconnected, got IP).
Your function decides what to do when each event happens.

Inside:

WIFI_EVENT_STA_START → Wi-Fi started → call esp_wifi_connect() to connect.

WIFI_EVENT_STA_DISCONNECTED → If disconnected → try to reconnect.

IP_EVENT_STA_GOT_IP → When the router gives ESP an IP → print the IP.

So this is how ESP “reacts” automatically when things happen.

3. Wi-Fi Initialization
void wifi_init_sta(void) {
    ESP_ERROR_CHECK(esp_netif_init());                // Initialize TCP/IP stack
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Event system
    esp_netif_create_default_wifi_sta();              // Create Wi-Fi Station (client)

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));             // Start Wi-Fi driver


This part:

Sets up network stack (so ESP32 knows how to talk TCP/IP).

Creates default Wi-Fi station (like WiFi.mode(WIFI_STA) in Arduino).

Initializes the Wi-Fi driver with default settings.

4. Register Events
ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));


Tells ESP-IDF:
“Hey, whenever something Wi-Fi related happens, call my event_handler function.”
