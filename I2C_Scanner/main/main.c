/*



#include "driver/i2c.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_SCL_IO    27
#define I2C_MASTER_SDA_IO    26
#define I2C_MASTER_NUM       I2C_NUM_0
#define I2C_MASTER_FREQ_HZ   100000
#define I2C_MASTER_TIMEOUT_MS 1000

void app_main(void) {
    // I2C কনফিগারেশন
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

    printf("🔍 Starting I2C Bus Scan...\n");

    int found = 0;
    for (uint8_t addr = 0x03; addr < 0x78; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK) {
            printf("✅ Found device at address: 0x%02X\n", addr);
            found++;
        }
    }

    if (found == 0) {
        printf("❌ No I2C devices found.\n");
    } else {
        printf("✅ Scan complete. %d device(s) found.\n", found);
    }
}

*/


/*


#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"

void scan_i2c(uint8_t sda, uint8_t scl) {
    printf("🔍 Trying SDA: GPIO%d, SCL: GPIO%d\n", sda, scl);

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000
    };
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);

    for (int i = 1; i < 127; i++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK) {
            printf("✅ Found I2C device at 0x%02X\n", i);
        }
    }
    i2c_driver_delete(I2C_NUM_0);
}

void app_main() {
    printf("\n🔍 Starting I2C Bus Scan on different GPIOs...\n");
    scan_i2c(26, 27);  // AI Thinker default
    scan_i2c(4, 15);   // Variant 1
    scan_i2c(21, 22);  // Variant 2
}

*/

#include "driver/i2c.h"
#include <stdio.h>

#define I2C_FREQ 100000  // 100kHz standard

// Function to scan with custom SDA/SCL
void scan_i2c(gpio_num_t sda, gpio_num_t scl) {
    printf("\n🔍 Trying SDA: GPIO%d, SCL: GPIO%d\n", sda, scl);

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ,
    };

    if (i2c_param_config(I2C_NUM_0, &conf) != ESP_OK) return;
    if (i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0) != ESP_OK) return;

    bool found = false;
    for (int addr = 1; addr < 127; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, addr << 1 | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK) {
            printf("✅ Device found at 0x%02X\n", addr);
            found = true;
        }
    }

    if (!found) {
        printf("❌ No I2C device found.\n");
    }

    i2c_driver_delete(I2C_NUM_0);
}

// Try all combinations
void app_main() {
    printf("🔎 Starting I2C Full Scan...\n");

    int sda_list[] = {4, 5, 12, 13, 14, 15, 18, 19, 21, 22, 25, 26, 27, 32, 33};
    int scl_list[] = {4, 5, 12, 13, 14, 15, 18, 19, 21, 22, 25, 26, 27, 32, 33};

    for (int i = 0; i < sizeof(sda_list)/sizeof(int); i++) {
        for (int j = 0; j < sizeof(scl_list)/sizeof(int); j++) {
            if (sda_list[i] == scl_list[j]) continue; // avoid same pin
            scan_i2c(sda_list[i], scl_list[j]);
        }
    }
}






