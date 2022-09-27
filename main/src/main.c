#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_cmds.h"
#include "can.h"
#include "nvs_flash.h"
#include "esp_log.h"



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

    //Initalize CAN
    can_init();

    while(true)
    {
    
    }

}