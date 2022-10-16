#ifndef CONFIG_H
#define CONFIG_H

#define BLINK_GPIO 25

//Wifi configs

#define WIFI_SSID "test"
#define WIFI_KEY "test"
#define WIFI_MAX_RETRY 3U

#define MULTICAST_IPV4_ADDR "224.1.1.1"
#define PORT 5007

// CAN configs
#define CAN_TX_GPIO (GPIO_NUM_21)
#define CAN_RX_GPIO (GPIO_NUM_22)

// Buffer configs
#define MAX_CAN_MSGS_PER_DATAGRAM (10UL * 64UL)

#define DATAGRAM_SOF (0xAAU)

#endif
