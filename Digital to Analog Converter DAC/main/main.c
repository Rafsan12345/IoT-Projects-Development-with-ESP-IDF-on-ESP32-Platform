

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/dac.h"
#include <math.h>

#define PI 3.14159265
#define DAC_CHANNEL DAC_CHAN_0   // GPIO25

void app_main() {
    dac_output_enable(DAC_CHANNEL);

    while (1) {
        for (int i = 0; i < 360; i++) {
            // sine value calculation: output = (sin(x) + 1) * 127.5 to convert -1~1 to 0~255
            float radians = i * PI / 180;
            uint8_t dac_value = (uint8_t)((sin(radians) + 1) * 127.5);
            dac_output_voltage(DAC_CHANNEL, dac_value);

            vTaskDelay(1 / portTICK_PERIOD_MS); // Adjust delay to control frequency
        }
    }
}






/*
#include "driver/dac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main() {
    // DAC_CHAN_0 - GPIO25, DAC_CHAN_1 - GPIO26
    dac_output_enable(DAC_CHAN_0);  // Enable DAC_CHAN_0 (GPIO25)
    dac_output_enable(DAC_CHAN_1);  // Enable DAC_CHAN_1 (GPIO26)

    while(1) {
        // Iterate over different voltage levels from 0 to 255
        for (int i = 0; i < 256; i++) {
            // Output voltage corresponding to the value of 'i' (0-255) on DAC_CHAN_0
            dac_output_voltage(DAC_CHAN_0, i);  
            // Output voltage corresponding to the value of 'i' (0-255) on DAC_CHAN_1
            dac_output_voltage(DAC_CHAN_1, 255 - i);  // Inverse pattern on DAC_CHAN_1
            
            // Delay to allow time for voltage change
            vTaskDelay(10 / portTICK_PERIOD_MS);  // Delay 10ms
        }
    }
}


*/

