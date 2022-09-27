#include "can.h"
#include <string.h>
#include "driver/gpio.h"
#include "driver/twai.h"
#include "freertos/FreeRTOS.h"
#include <time.h>
#include "config.h"
#include "esp_log.h"

// @NOTE CAN is apparently TWAI in esp land
// TWAI = two wire automotive interface (this is so generic ugh)

static const char* TAG = "can.c";


void can_init(void)
{
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_GPIO, CAN_RX_GPIO, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    if(twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        ESP_LOGI(TAG, "CAN driver installed");
    }
    else
    {
        ESP_LOGE(TAG, "CAN driver install failed");
        return;
    }
    if(twai_start() == ESP_OK)
    {
        ESP_LOGI(TAG, "CAN started");
    }
    else
    {
        ESP_LOGE(TAG, "CAN start failed");
        return;
    }
}

void can_send(sunstream_can_msg_t msg)
{
    twai_message_t can_msg = {0};
    can_msg.identifier = msg.id;
    can_msg.data_length_code = msg.len;
    can_msg.flags = TWAI_MSG_FLAG_NONE;
    memcpy(can_msg.data, msg.data, msg.len);
    if(twai_transmit(&can_msg, pdMS_TO_TICKS(100)) == ESP_OK)
    {
        ESP_LOGI(TAG, "CAN message sent");
    }
    else
    {
        ESP_LOGE(TAG, "CAN message send failed");
    }
}

sunstream_can_msg_t can_receive(void)
{
    sunstream_can_msg_t msg = {0};
    twai_message_t can_msg = {0};
    if(twai_receive(&can_msg, pdMS_TO_TICKS(100)) == ESP_OK)
    {
        struct tm ts = {0};
        //Get the epoch time
        time_t epoch_ts = mktime(&ts);
        msg.timestamp = epoch_ts;
        msg.id = can_msg.identifier;
        msg.len = can_msg.data_length_code;
        memcpy(msg.data, can_msg.data, msg.len);
        ESP_LOGI(TAG, "CAN message received");
    }
    else
    {
        ESP_LOGE(TAG, "CAN message receive failed");
    }
    return msg;
}
