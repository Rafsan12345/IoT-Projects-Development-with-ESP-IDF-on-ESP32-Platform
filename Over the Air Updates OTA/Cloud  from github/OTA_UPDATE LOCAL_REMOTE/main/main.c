#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define SSID "UIU-STUDENT"
#define PASS "12345678"
#define OTA_URL "https://raw.githubusercontent.com/Rafsan12345/esp32_ota/main/ABC.bin"

extern const uint8_t github_cert_start[] asm("_binary_github_cert_pem_start");
extern const uint8_t github_cert_end[]   asm("_binary_github_cert_pem_end");

static const char *TAG = "OTA_UPDATE";

void wifi_init() {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = SSID,
            .password = PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "Connecting to Wi-Fi...");
    esp_wifi_connect();
}

void ota_update_task(void *pvParameter) {
    esp_http_client_config_t config = {
        .url = OTA_URL,
        .cert_pem = (const char *) github_cert_start, // cast added
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };

    ESP_LOGI(TAG, "Starting OTA from GitHub...");
    esp_err_t ret = esp_https_ota(&ota_config);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA Update Successful. Restarting...");
        esp_restart();
    } else {
        ESP_LOGE(TAG, "OTA Update Failed! Error: %s", esp_err_to_name(ret));
    }

    vTaskDelete(NULL);
}

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();
    vTaskDelay(5000 / portTICK_PERIOD_MS); // wait for Wi-Fi

    xTaskCreate(&ota_update_task, "ota_update_task", 16384, NULL, 5, NULL);
}
