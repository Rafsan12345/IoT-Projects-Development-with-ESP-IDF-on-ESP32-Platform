#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#define SERVER_IP   "10.15.43.168"
#define SERVER_PORT 5000

void tcp_client_task(void *pvParameters)
{
    char send_buf[] = "Hello Server!";
    char recv_buf[128];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        ESP_LOGE("TCP", "Unable to create socket");
        vTaskDelete(NULL);
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE("TCP", "Connection failed");
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI("TCP", "Connected to server");

    send(sock, send_buf, strlen(send_buf), 0);
    int len = recv(sock, recv_buf, sizeof(recv_buf) - 1, 0);
    if (len > 0) {
        recv_buf[len] = 0;
        ESP_LOGI("TCP", "Received: %s", recv_buf);
    }

    close(sock);
    vTaskDelete(NULL);
}

void app_main()
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "UIU-STUDENT",
            .password = "12345678",
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();

    vTaskDelay(5000 / portTICK_PERIOD_MS); // wait for WiFi

    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
}
