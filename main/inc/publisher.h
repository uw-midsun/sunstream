#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "can.h"
#include "config.h"

typedef struct __attribute__((packed))
{
    uint8_t SOF;
    uint16_t msg_count; // number of messages in the buffer
} sunstream_datagram_header_t;

typedef struct __attribute__((packed))
{
    sunstream_datagram_header_t header;
    sunstream_can_msg_t msgs[MAX_CAN_MSGS_PER_DATAGRAM];
} sunstream_datagram_t;

/**
 * @brief Appends a CAN message to the buffer
 * 
 * @param msg 
 */
void append_can_msg_to_buffer(sunstream_can_msg_t msg);

/**
 * @brief Publishes the can msg buffer
 * 
 */
void publish_datagram();


#endif // PUBLISHER_H
