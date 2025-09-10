#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "inttypes.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"



#define interrupt_pin        5


static void IRAM_ATTR gpio_isr_handler(void *arg)
{
 
    ESP_EARLY_LOGI("GPIO", "Interrupt Button Pressed!");

}



#define GPIO_OUTPUT_PIN 2

void app_main(void)
{
	
	
	///
	    gpio_reset_pin(GPIO_OUTPUT_PIN);
    gpio_set_direction(GPIO_OUTPUT_PIN, GPIO_MODE_OUTPUT);

	
	///
    gpio_reset_pin(interrupt_pin);
    gpio_set_direction(interrupt_pin, GPIO_MODE_INPUT);

    gpio_set_pull_mode(interrupt_pin, GPIO_PULLUP_ONLY);

    gpio_set_intr_type(interrupt_pin, GPIO_INTR_POSEDGE);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(interrupt_pin, gpio_isr_handler, NULL);

    gpio_intr_enable(interrupt_pin);

    while(1)
    {
        gpio_set_level(GPIO_OUTPUT_PIN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_PIN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);


    }

}