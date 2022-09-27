#ifndef CONFIG_H
#define CONFIG_H

#define BLINK_GPIO 25

//Wifi configs

#define WIFI_SSID "SSID"
#define WIFI_KEY "PASS"
#define WIFI_MAX_RETRY 3U

#define MULTICAST_IPV4_ADDR "232.10.11.12"
#define PORT 3333

// CAN configs
#define CAN_TX_GPIO (GPIO_NUM_21)
#define CAN_RX_GPIO (GPIO_NUM_22)

#endif
