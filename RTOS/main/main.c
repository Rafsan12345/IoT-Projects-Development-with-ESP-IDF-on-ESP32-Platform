/* main.c
   Simple FreeRTOS Example: 2 Tasks on ESP32 WROOM32
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

static const char *TAG = "RTOS_EXAMPLE";

/* Task 1: Print every 1 second */
void task1(void *pvParameters)
{
    while (1) {
        ESP_LOGI(TAG, "Task 1: Hello from Task 1!");
        vTaskDelay(pdMS_TO_TICKS(1000));  // 1 second delay
    }
}

/* Task 2: Print every 2 seconds */
void task2(void *pvParameters)
{
    while (1) {
        ESP_LOGI(TAG, "Task 2: Hello from Task 2!");
        vTaskDelay(pdMS_TO_TICKS(2000));  // 2 second delay
    }
}

/* Main application */
void app_main(void)
{
    ESP_LOGI(TAG, "Starting 2 FreeRTOS tasks...");

    /* Create Task 1 */
    xTaskCreate(task1,       // function
                "Task1",     // name
                2048,        // stack size in words
                NULL,        // parameters
                5,           // priority
                NULL);       // task handle

    /* Create Task 2 */
    xTaskCreate(task2,
                "Task2",
                2048,
                NULL,
                5,
                NULL);
}
