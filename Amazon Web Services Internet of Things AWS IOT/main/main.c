#include <stdio.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// WiFi Credentials
#define WIFI_SSID "UIU-Faculty-Staff"       // Replace with your WiFi SSID
#define WIFI_PASSWORD "UIU#9876"            // Replace with your WiFi Password

// AWS IoT Configuration
#define AWS_IOT_ENDPOINT "a30c7a4mkfkv2f-ats.iot.ap-southeast-2.amazonaws.com:8883" // Replace with your AWS IoT endpoint




extern const uint8_t client_cert_pem_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_certificate_pem_crt_end");

extern const uint8_t client_key_pem_start[] asm("_binary_private_pem_key_start");
extern const uint8_t client_key_pem_end[] asm("_binary_private_pem_key_end");

extern const uint8_t server_cert_pem_start[] asm("_binary_AmazonRootCA1_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_AmazonRootCA1_pem_end");




#define CLIENT_CERTIFICATE "-----BEGIN CERTIFICATE-----\
MIIDWTCCAkGgAwIBAgIUWnrIHHh828cun/i+pYPlwRZoFTIwDQYJKoZIhvcNAQEL\
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDExOTExMTg1\
MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJ/KiMd5G6AMiD1orRvT\
Da7tJtn2yAXO4wHQYD\
VR0OBBYEFGnfC5bQ6fl1nnLDHPoRT5EfEfO5MAwGA1UdEwEB/wQCMAAwDgYDVR0P\
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQDSpbNj7dlMIAsdkVyCnlngNxOY\
uJcApg9WKJAVcx+6rrKWMkjNYhPKb7vcLAVEk6nGEpeflR3c3j5heO8n8l/kqEYL\
YSKGCzuy1v0gYUrvTCmXiyexiZaEHzNnRoOmbmRXbPujRSg2TIY413kTSLFHWOGM\
RR71Xlj+ymqggP5IOph5Z5Qta2nyEH1vNW2IDiTQTMhpwdIj3uGerZBCN+SzkNbx\
xaE68JawmhhGC4wqVM2q+p4nUnFR7mvWDJKbfv4l0v5TjevujrWE5NAJzk0SnwAC\
BOHBwHdgJoPkChMk6zZ9drdfAYqkrR9N27XQWU2LduBcN28y+6qfnimqY6oQ\
-----END CERTIFICATE-----\
"


#define CLIENT_PRIVATE_KEY "-----BEGIN RSA PRIVATE KEY-----\
MIIEowIBAAKCAQEAwH3mlgxEZJZqfGiUxfk/VYllRtMmuZ/NAievCnqKmTmwJRAv\
TS/TWRikXOKsgboB+9Ccpsxxn6dbpuGQACM6kJG8949dQYafahFk5EYdaUGwqaMC\
9VTPthINmPM7cK+nP2QXqJ+ey8S4b3dSorjt8Bn7jxYrzQfzvF9vK1XfRRMIgVtX\
gHcgukkXt3fsJKt7bYI7njI8/Bs\
uMC2BzwuS6inDUdUqAkv7NLeci39/AwpiyLYKBclS5iTQo3/sG5gOVRdkcpC9ygN\
62dUXtB5T5UFRV/aMCI+L7bNntoir5l8NLTtFYsCgYEApo0iBatXHesE0aizjcqi\
4JHKs7zUa7fida0HeoA+z1p5qr96MLFeZB5dvhx3UwqcYAKWOH7uQ/fpFdqvR9bB\
OLDfS/2TnzQNWNpjt/Ei1CkNFhOShV0y7LnacxIPPa01DnC4LHT7ElZEJPiHiReW\
YGfu3PP1P1bGsB8XBmWU1zECgYBbVphYCK5tMNtiqpoQZARqYhKgLGJGw9voQQps\
TpCw7PRv5XFXHU8fYHxbDSdtDmFso9ZtpBHHsOinVBdzhwcBExDLdLhHa6P+VG1h\
vFG3b0B1V34zC6pWte23jC0WJHBz3Ns09K80ecgplwFpNX4QBQheRVNSoz8xERzp\
cOMWzQKBgFclE794wz+YifyoJev5wf+p45TIE73V7M+HwiobtlupFK3tkQ40i+hx\
jpU11Xl1HP76I6jktHnIBlIMgF2Cj6lY0r7zEoN+gAfMmcJAAaxDXiyncA+wZWPS\
MNzhgKbBX6d1XdfejJI7C0jkmRMLCHo2sm3Ar1ex2raBwnifJ31+\
-----END RSA PRIVATE KEY-----\
"




#define ROOT_CA "-----BEGIN CERTIFICATE-----\
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\
b3QgQ096LXFvKWlJbYK8U90vv\
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\
rqXRfboQnoZsG4q5WTP468SQvvG5\
-----END CERTIFICATE-----"                                

static const char *TAG = "MQTT";

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

// Initialize WiFi connection
static void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));



    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}

// MQTT event handler with corrected signature
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch (event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT Connected to broker");
        
        msg_id = esp_mqtt_client_subscribe(client, "/topic/test", 0);
        ESP_LOGI(TAG, "Sent subscribe message, msg_id=%d", msg_id);
        
        vTaskDelay(2000 / portTICK_PERIOD_MS); // 5-second delay
        
        msg_id = esp_mqtt_client_publish(client, "/topic/test", "Hello AWS IoT", 0, 1, 0);
        ESP_LOGI(TAG, "Sent publish message, msg_id=%d", msg_id);

        
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT Disconnected from broker");
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA received");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;

    default:
        ESP_LOGI(TAG, "Unhandled event id: %d", (int)event_id);
        break;
    }
}

// MQTT configuration
static esp_mqtt_client_config_t mqtt_cfg = {
	//.uri = "mqtts://<ENDPOINT>...st-1.amazonaws.com:8883",
	
    .broker.address.uri = "mqtts://" AWS_IOT_ENDPOINT,
    .broker.verification.certificate = (const char *)server_cert_pem_start,
    .credentials.authentication.certificate = (const char *)client_cert_pem_start,
    .credentials.authentication.key = (const char *)client_key_pem_start,
    
};






// Initialize and start MQTT client
void mqtt_app_start(void)
{
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client));
    ESP_ERROR_CHECK(esp_mqtt_client_start(client));
}

// Main application
void app_main(void)
{
	ESP_LOGI(TAG, "Starting........................................xxxx777");
    ESP_ERROR_CHECK(nvs_flash_init()); // Initialize NVS
    wifi_init();                      // Initialize WiFi
    mqtt_app_start();                 // Start MQTT
}
