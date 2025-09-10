/* coap_server_minimal.c
   সরল CoAP server: /hello রিসোর্সে GET করলে static message পাঠাবে।
   ESP32 WROOM32, ESP-IDF v5.3.1, espressif/coap component
*/

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "coap3/coap.h"   // ESP-IDF coap header

static const char *TAG = "COAP_SERVER";

/* WiFi credentials */
#define WIFI_SSID "UIU-STUDENT"
#define WIFI_PASS "12345678"

static EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

/* WiFi event handler */
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WiFi disconnected, retry...");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "WiFi connected");
    }
}

/* Initialize WiFi as STA */
static void wifi_init_sta(void)
{
    wifi_event_group = xEventGroupCreate();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .pmf_cfg = {.capable = true, .required = false},
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
}

/* --- CoAP GET handler --- */
static void hnd_get_hello(coap_resource_t *resource,
                          coap_session_t *session,
                          const coap_pdu_t *request,
                          const coap_string_t *query,
                          coap_pdu_t *response)
{
    const char *msg = "Hello from ESP32 CoAP!";
    size_t len = strlen(msg);

    coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);

    /* Content-Format: text/plain (0) */
    unsigned char buf[3];
    coap_add_option(response, COAP_OPTION_CONTENT_FORMAT,
                    coap_encode_var_safe(buf, sizeof(buf), COAP_MEDIATYPE_TEXT_PLAIN),
                    buf);

    coap_add_data(response, len, (const uint8_t *)msg);
}

/* CoAP server task */
void coap_server_task(void *pvParameters)
{
    coap_context_t *ctx = NULL;
    coap_address_t serv_addr;
    coap_endpoint_t *endpoint = NULL;
    coap_resource_t *resource = NULL;

    /* Initialize server address */
    coap_address_init(&serv_addr);
    serv_addr.addr.sin.sin_family      = AF_INET;
    serv_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;
    serv_addr.addr.sin.sin_port        = htons(5683);

    ctx = coap_new_context(NULL);
    if (!ctx) {
        ESP_LOGE(TAG, "coap_new_context failed");
        vTaskDelete(NULL);
        return;
    }

    /* Create UDP endpoint */
    endpoint = coap_new_endpoint(ctx, &serv_addr, COAP_PROTO_UDP);
    if (!endpoint) {
        ESP_LOGE(TAG, "coap_new_endpoint failed");
        coap_free_context(ctx);
        vTaskDelete(NULL);
        return;
    }

    /* Create /hello resource */
    resource = coap_resource_init(coap_make_str_const("hello"), 0);
    coap_register_handler(resource, COAP_REQUEST_GET, hnd_get_hello);
    coap_add_resource(ctx, resource);

    ESP_LOGI(TAG, "CoAP server started, resource: /hello");

    while (1) {
        coap_run_once(ctx, 1000);
    }

    coap_free_context(ctx);
    vTaskDelete(NULL);
}

/* app_main */
void app_main(void)
{
    ESP_LOGI(TAG, "Initializing NVS");
    nvs_flash_init();
    wifi_init_sta();

    /* Start CoAP server task */
    xTaskCreate(coap_server_task, "coap_server", 4096, NULL, 5, NULL);
}
