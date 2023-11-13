#include "DataLinkLevel/eth_prot.h"

extern char str1[60];
extern UART_HandleTypeDef huart2;
extern uint8_t macaddr[6];
extern USART_prop_ptr usartprop;

void eth_read(enc28j60_frame_ptr *frame, uint16_t len)
{
    uint8_t res = 0;

    if (len >= sizeof(enc28j60_frame_ptr))
    {
        sprintf(str1, "%02X:%02X:%02X:%02X:%02X:%02X-%02X:%02X:%02X:%02X:%02X:%02X; %d; %04Xrn",

                frame->addr_src[0], frame->addr_src[1], frame->addr_src[2], frame->addr_src[3], frame->addr_src[4], frame->addr_src[5],

                frame->addr_dest[0], frame->addr_dest[1], frame->addr_dest[2], frame->addr_dest[3], frame->addr_dest[4], frame->addr_dest[5], len, be16toword(frame->type));
        if (frame->type == ETH_ARP)
        {
            HAL_UART_Transmit(&huart2, (uint8_t *)str1, strlen(str1), 0x1000);
            res = arp_read(frame, len - sizeof(enc28j60_frame_ptr));
            if (res == 1)
            {
                arp_reply(frame);
            }
            else if (res == 2)
            {
                arp_table_fill(frame);
                if (usartprop.send_type == UDP_SEND) {
                    memcpy(frame->addr_dest, frame->addr_src, 6);
                    net_cmd();
                }
            }
        }

        if (frame->type == ETH_IP)
        {
            ip_read(frame, len - sizeof(ip_pkt_ptr));
        }
    }
}

void eth_send(enc28j60_frame_ptr *frame, uint16_t len)
{
    // memcpy(frame->addr_dest, frame->addr_src, 6);
    memcpy(frame->addr_src, macaddr, 6);
    enc28j60_packetSend((void *)frame, len + sizeof(enc28j60_frame_ptr));
}