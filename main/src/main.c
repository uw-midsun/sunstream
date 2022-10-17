#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_cmds.h"
#include "publisher.h"
#include "can.h"
#include "nvs_flash.h"
#include <sys/time.h>
#include "esp_log.h"
#include <stdlib.h>

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
    //Seed random number generator
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);
    while(true)
    {
        sunstream_can_msg_t msg;
        //Get the epoch time
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);
        int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
        msg.timestamp = time_us;
        // Generate a random CAN ID
        msg.id = rand() % 0x7FF;
        // Generate random length capping at 8
        msg.len = rand() % 8;
        //Generate random data and put into the message
        for(int i = 0; i < msg.len; i++)
        {
            msg.data[i] = rand() % 256;
        }
        append_can_msg_to_buffer(msg);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        // Print out in hex the spoofed can message, data and timestamp
        /*
        uint8_t* data = (uint8_t*)&msg;
        printf("CAN Message: ");
        for(int i = 0; i < sizeof(msg); i++)
        {
            printf("%02X ", data[i]);
        }
        printf("\n");
        */
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
    ESP_LOGI("main", "Publish task created, size of datagram: %d, size of datagram header: %d, size of can message: %d", 
    sizeof(sunstream_datagram_t), sizeof(sunstream_datagram_header_t), sizeof(sunstream_can_msg_t));
    #ifndef DEBUG_SPOOF_CAN
    xTaskCreate(&can_task, "can_task", 4096, NULL, 0, NULL);
    #else
    //Initalize CAN
    can_init();
    xTaskCreate(&spoof_can_task, "spoof_can_task", 4096, NULL, 0, NULL);
    #endif
}