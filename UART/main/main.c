
//Transmit Data 

/*

#include <stdio.h>
#include "driver/uart.h"
#include <string.h>

#define UART_NUM UART_NUM_1
#define TX_PIN 17
#define RX_PIN 16
#define BUF_SIZE 1024

void app_main() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_driver_install(UART_NUM, BUF_SIZE, BUF_SIZE, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    const char *message = "Hello, UART!\n";
    uart_write_bytes(UART_NUM, message, strlen(message));
}


*/




// Receive and transmit both 





#include <stdio.h>
#include "driver/uart.h"
#include <string.h>

#define UART_NUM UART_NUM_1
#define TX_PIN 17
#define RX_PIN 16
#define BUF_SIZE 1024

void app_main() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    
    // UART ড্রাইভার ইনস্টল করা
    uart_driver_install(UART_NUM, BUF_SIZE, BUF_SIZE, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // পাঠানোর বার্তা
    const char *message = "Hello, UART!\n";
    uart_write_bytes(UART_NUM, message, strlen(message));  // TX

    // RX থেকে ডেটা পড়া
    uint8_t data[BUF_SIZE];
    while (1) {
        // UART RX বাফার থেকে ডেটা পড়া
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, pdMS_TO_TICKS(100)); // 100 ms timeout
        if (len > 0) {
            // প্রাপ্ত ডেটা প্রিন্ট করা
            data[len] = '\0';  // Null terminate the received string
            printf("Received: %s\n", data);  // RX ডেটা প্রিন্ট করা
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // প্রতিটি রিড অপারেশনের মাঝে 100ms দেরি
    }
}



