#include "esp_http_client.h"
#include "esp_ota_ops.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_https_ota.h"


#define SSID "UIU-STUDENT"
#define PASS "12345678"

#define OTA_URL "http://10.15.43.168:8080/ABC.bin"  // 🔹 HTTP সার্ভার থেকে ফার্মওয়্যার URL


//http://10.10.201.63:8080/ABC.bin

static const char *TAG = "OTA_UPDATE";







// ✅ Wi-Fi কানেকশন ফাংশন
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

// ✅ OTA আপডেট ফাংশন
void ota_update_task(void *pvParameter) {
    esp_http_client_config_t config = {
        .url = OTA_URL,
        .cert_pem = (char *)"",
        .timeout_ms = 50000, // 5 সেকেন্ড টাইমআউট
    };



 esp_https_ota_config_t ota_config = {
        .http_config = &config, // ✅ এখানে সঠিকভাবে struct পাস করা হয়েছে
    };

    
     esp_err_t ret = esp_https_ota(&ota_config); // ✅ এখন কাজ করবে
    
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA Update Successful. Restarting...");
        esp_restart();  // আপডেট সফল হলে ESP32 রিস্টার্ট করবে
    } else {
        ESP_LOGE(TAG, "OTA Update Failed!");
        ESP_LOGE("OTA_UPDATE", "OTA Update Failed! Error Code: %s", esp_err_to_name(ret));
    }

    vTaskDelete(NULL);
}

// ✅ মেইন ফাংশন (Wi-Fi + OTA রান করবে)
void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();  // 🔹 Wi-Fi কানেক্ট করুন
    vTaskDelay(5000 / portTICK_PERIOD_MS); // 🔹 Wi-Fi কানেকশন স্থিতিশীল করতে Delay

    xTaskCreate(&ota_update_task, "ota_update_task", 16384, NULL, 5, NULL);
}
