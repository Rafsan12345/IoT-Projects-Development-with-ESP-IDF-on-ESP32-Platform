#include <stdio.h>
#include "driver/gpio.h"
#include <unistd.h>

#define LED1 GPIO_NUM_18
#define LED2 GPIO_NUM_19
#define BUTTON1 GPIO_NUM_4
#define BUTTON2 GPIO_NUM_5



void app_main() {
    gpio_config_t io_conf = {};

    // Output LEDs configuration
    io_conf.pin_bit_mask = (1ULL << LED1) | (1ULL << LED2);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    // Input Buttons configuration with pull-up
    io_conf.pin_bit_mask = (1ULL << BUTTON1) | (1ULL << BUTTON2);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;    // pull-up active
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;      // rising edge interrupt
    gpio_config(&io_conf);

    printf("GPIOs configured!\n");

    while (1) {
        // LEDs ON/OFF loop
        gpio_set_level(LED1, 1);
        gpio_set_level(LED2, 0);
        sleep(1);
        gpio_set_level(LED1, 0);
        gpio_set_level(LED2, 1);
        sleep(1);

        // Button state read
        int btn1 = gpio_get_level(BUTTON1);
        int btn2 = gpio_get_level(BUTTON2);
        printf("Button1: %d, Button2: %d\n", btn1, btn2);
    }
}
