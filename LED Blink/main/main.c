

#include <stdio.h>
#include "driver/gpio.h"
#include <stdio.h>
#include "esp_timer.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>


#define LED_ON GPIO_NUM_18   // LED

void app_main() {

  gpio_set_direction(LED_ON, GPIO_MODE_OUTPUT);
 
 
 printf("Starting\n");
 
   while (1) {
          gpio_set_level(LED_ON, 1);   // LED_ON চালু হবে
          sleep(1);
          gpio_set_level(LED_ON, 0);   // LED_ON বন্ধ হবে
          sleep(1);

  }
}
