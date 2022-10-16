#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_cmds.h"
#include "publisher.h"
#include "can.h"
#include "nvs_flash.h"
#include <time.h>
#include "esp_log.h"

#define DEBUG_SPOOF_CAN

void publish_task(void* pvParameters)
{   
    while(true)
    {
        publish_datagram();
        vTaskDelay(50 / portTICK_PERIOD_MS); //2Hz publish rate
    }
}

void can_task(void* pvParameters)
{
    //Initalize CAN
    can_init();
    while(true)
    {
        //Blocking call to just lazily recieve CAN messages
        sunstream_can_msg_t msg = can_receive();
        append_can_msg_to_buffer(msg);
    }
}

#ifdef DEBUG_SPOOF_CAN
void spoof_can_task(void* pvParameters)
{
    //ESP_LOGI("main", "Datagram message size: %d", sizeof(sunstream_datagram_t));
    while(true)
    {
        sunstream_can_msg_t msg;
        struct tm ts = {0};
        //Get the epoch time
        time_t epoch_ts = mktime(&ts);
        msg.timestamp = epoch_ts;
        msg.id = 0x123;
        msg.data[0] = 0x01;
        msg.data[1] = 0x02;
        msg.data[2] = 0x03;
        msg.data[3] = 0x04;
        msg.data[4] = 0x05;
        msg.data[5] = 0x06;
        msg.data[6] = 0x07;
        msg.data[7] = 0x08;
        msg.len = 8;
        append_can_msg_to_buffer(msg);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
#endif


void app_main(void)
{

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    //Initalize wifi
    if(wifi_init_sta())
    {
        ESP_LOGE("main", "Failed to initialize wifi");
    }
    else
    {
        ESP_LOGI("main", "Wifi initialized");
    }

    xTaskCreate(&publish_task, "publish_task", 4096, NULL, 1, NULL);
    ESP_LOGI("main", "Publish task created, size of datagram: %d", sizeof(sunstream_datagram_t));
    #ifndef DEBUG_SPOOF_CAN
    xTaskCreate(&can_task, "can_task", 4096, NULL, 0, NULL);
    #else
    //Initalize CAN
    can_init();
    xTaskCreate(&spoof_can_task, "spoof_can_task", 4096, NULL, 0, NULL);
    xTaskCreate(&spoof_can_task, "spoof_can_task", 4096, NULL, 0, NULL);
    xTaskCreate(&spoof_can_task, "spoof_can_task", 4096, NULL, 0, NULL);
    #endif
}