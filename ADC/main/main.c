#include <stdio.h>
#include "driver/adc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#define TAG "ADC_EXAMPLE"

void app_main() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_12);

    while (1) {
        int raw_value = adc1_get_raw(ADC1_CHANNEL_6);
        ESP_LOGI(TAG, "ADC Raw Value: %d", raw_value);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
