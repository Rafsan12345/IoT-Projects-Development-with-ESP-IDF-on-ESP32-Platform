#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"

static bool IRAM_ATTR timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    //printf("Timer fired! Count: %lld us\n", edata->count_value);
    esp_rom_printf("Timer Callback!\n");
    return true; // Return true to yield from ISR
}

void app_main(void)
{
    // Timer configuration
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,  // Usually APB
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz = 1 tick = 1us
    };
    gptimer_new_timer(&config, &gptimer);

    // Set up the alarm
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1000000,  // 1 second = 1,000,000 us
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_set_alarm_action(gptimer, &alarm_config);

    // Register the callback
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_callback,
    };
    gptimer_register_event_callbacks(gptimer, &cbs, NULL);

    // Enable and start the timer
    gptimer_enable(gptimer);
    gptimer_start(gptimer);
    
    
    printf("START!!!!");
}
