// Input to HTML Server for ESP32
#include <stdio.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "my_data.h"

#include "driver/gpio.h"

#include "dht11.h"

#define GPIO_OUTPUT_PIN 2

#define DHT_GPIO 4  // DHT11 সংযুক্ত GPIO পিন (যেমন, GPIO4)



static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_connection()
{
    // 1 - Wi-Fi/LwIP Init Phase
    esp_netif_init();                    // TCP/IP initiation 					s1.1
    esp_event_loop_create_default();     // event loop 			                s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
}

esp_err_t get_handler(httpd_req_t *req) {
	

char html_page[] =
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"<title>ESP32 Web Server</title>\n"
"</head>\n"
"<body>\n"
"<h1>ESP32 Web Server</h1>\n"
"<h2>Temperature & Humidity</h2>\n"
"<p>Temperature: %.2fC</p>\n"
"<p>Humidity: %.2f %%</p>\n"
"<h2>LED Control</h2>\n"
"<a href=\"/led_on\"><button>Turn LED ON</button></a>\n"
"<a href=\"/led_off\"><button>Turn LED OFF</button></a>\n"
"</body>\n"
"</html>";

/*
printf("Temperature is %d \n", DHT11_read().temperature);
printf("Humidity is %d\n", DHT11_read().humidity);
*/

   // x++;
    
    float temperature = DHT11_read().temperature;  // সেন্সর থেকে পাওয়া তাপমাত্রা (উদাহরণ)
    float humidity = DHT11_read().humidity;     // সেন্সর থেকে পাওয়া আর্দ্রতা (উদাহরণ)
    
    char response[512];
    snprintf(response, sizeof(response), html_page, temperature, humidity);

    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}



esp_err_t Led_on(httpd_req_t *req1)
 {
	///led_on?
	gpio_set_level(GPIO_OUTPUT_PIN, 1);
	printf("LED ON  \n");
	    // The response
	    
    const char resp[] = "LED ON ...";
    httpd_resp_send(req1, resp, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}


esp_err_t Led_off(httpd_req_t *req2)
 {
	////led_off?
	gpio_set_level(GPIO_OUTPUT_PIN, 0);
	printf("LED OFF\n");
	
    const char resp[] = "LED OFF ...";
    httpd_resp_send(req2, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}




/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_handler,
    .user_ctx = NULL};

httpd_uri_t uri_led_on = {
    .uri = "/led_on",
    .method = HTTP_GET,
    .handler = Led_on,
    .user_ctx = NULL};
    
  httpd_uri_t uri_led_off = {
    .uri = "/led_off",
    .method = HTTP_GET,
    .handler = Led_off,
    .user_ctx = NULL};  
    
    
    
    

httpd_handle_t start_webserver(void)
{
	
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK)
    {
		printf("Start Webserver \n");
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_led_on);
        httpd_register_uri_handler(server, &uri_led_off);
    }
    

    return server;
}

void stop_webserver(httpd_handle_t server)
{
    if (server)
    {
        httpd_stop(server);
    }
}

void app_main(void)
{
	DHT11_init(DHT_GPIO);
	//DHT11_read().temperature
	//DHT11_read().humidity
	
	gpio_set_direction(GPIO_OUTPUT_PIN, GPIO_MODE_OUTPUT);
	
    nvs_flash_init();
    wifi_connection();
    start_webserver();
    
    
    
}