#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "SPIFFS_READ";

void app_main(void)
{
    // SPIFFS কনফিগার এবং মাউন্ট করুন
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",               // ফাইল সিস্টেমের মূল পাথ
        .partition_label = NULL,             // পার্টিশনের লেবেল
        .max_files = 5,                      // সর্বোচ্চ ফাইল সংখ্যা
        .format_if_mount_failed = true       // মাউন্ট ব্যর্থ হলে ফরম্যাট করুন
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Faild................................");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "SPIFFS NO partition storage........................");
        } else {
            ESP_LOGE(TAG, "SPIFFS Faild (%s)", esp_err_to_name(ret));
        }
        return;
    }

    ESP_LOGI(TAG, "SPIFFS Sucessfulll.......................");

    // ফাইল পড়া এবং প্রিন্ট করার লুপ
    while (true) {
        FILE* f = fopen("/spiffs/A.txt", "r");
        if (f == NULL) {
            ESP_LOGE(TAG, "faild open file ");
        } else {
            ESP_LOGI(TAG, "Sucessfully open ");
            char line[128];
            while (fgets(line, sizeof(line), f) != NULL) {
                printf("%s", line); // ফাইলের কনটেন্ট প্রিন্ট করুন
            }
            fclose(f);
        }
        sleep(1); // ১ সেকেন্ড অপেক্ষা
    }

    // SPIFFS আনমাউন্ট করুন (প্রয়োজন হলে)
    esp_vfs_spiffs_unregister(NULL);
    ESP_LOGI(TAG, "Start System");
}
