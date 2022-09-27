#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_cmds.h"

void app_main(void)
{
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