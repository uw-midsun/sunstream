#include "publisher.h"
#include "wifi_cmds.h"

static sunstream_datagram_t datagram_buffer;

void append_can_msg_to_buffer(sunstream_can_msg_t msg)
{
    if(datagram_buffer.header.msg_count < MAX_CAN_MSGS_PER_DATAGRAM)
    {
        msg.magic_number = CAN_SOF;
        datagram_buffer.msgs[datagram_buffer.header.msg_count] = msg;
        datagram_buffer.header.msg_count++;
    }
}

void publish_datagram(void)
{
    datagram_buffer.header.SOF = DATAGRAM_SOF; //Magic number
    if(datagram_buffer.header.msg_count > 0)
    {
        send_udp_packet((uint8_t*)&datagram_buffer, sizeof(datagram_buffer));
        datagram_buffer.header.msg_count = 0;
    }
}