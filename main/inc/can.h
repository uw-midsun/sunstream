#ifndef CAN_H
#define CAN_H

#include <stdint.h>

typedef struct __attribute__((packed)) 
{
    int64_t timestamp;
    uint32_t id;
    uint8_t len;
    uint8_t data[8];
} sunstream_can_msg_t;

/**
 * @brief Initalize the CAN peripheral
 * 
 */
void can_init(void);

/**
 * @brief Blocking function to receive a CAN message
 * 
 * @return sunstream_can_msg_t can message received
 */
sunstream_can_msg_t can_receive(void);

/**
 * @brief Blocking function to send a CAN message
 * 
 * @param msg sunstream_can_msg_t message to send
 */
void can_send(sunstream_can_msg_t msg);



#endif
