#include "arp.h"

extern UART_HandleTypeDef huart2;
extern uint8_t ipaddr[4];
extern uint8_t macaddr[6];
extern char str1[60];
extern uint8_t net_buf[ENC28J60_MAXFRAME];
uint8_t macbroadcast[6] = MAC_BROADCAST;
uint8_t macnull[6] = MAC_NULL;

uint8_t arp_read(enc28j60_frame_ptr *frame, uint16_t len)
{
    uint8_t res = 0;
    arp_msg_ptr *msg = (void *)(frame->data);
    if (len >= sizeof(arp_msg_ptr))
    {
        if ((msg->net_tp == ARP_ETH) && (msg->proto_tp == ARP_IP))
        {

            if ((msg->op == ARP_REQUEST) && (!memcmp(msg->ipaddr_dst, ipaddr, 4)))
            {
                if ((msg->op == ARP_REQUEST) && (!memcmp(msg->ipaddr_dst, ipaddr, 4)))
                {
                    res = 1;
                }
            }
        }
    }

    return res;
}

void arp_send(enc28j60_frame_ptr *frame)
{
    arp_msg_ptr *msg = (void *)frame->data;
    msg->op = ARP_REPLY;
    msg->op = ARP_REPLY;
    memcpy(msg->macaddr_dst, msg->macaddr_src, 6);
    memcpy(msg->macaddr_src, macaddr, 6);
    memcpy(msg->macaddr_src, macaddr, 6);
    memcpy(msg->ipaddr_dst, msg->ipaddr_src, 4);
    memcpy(msg->ipaddr_src, ipaddr, 4);
    memcpy(msg->ipaddr_src, ipaddr, 4);

    eth_send(frame, sizeof(arp_msg_ptr));
}

uint8_t arp_request(uint8_t *ip_addr)
{
    uint8_t i;
    enc28j60_frame_ptr *frame = (void *)net_buf;
    arp_msg_ptr *msg = (void *)frame->data;
    msg->net_tp = ARP_ETH;
    msg->proto_tp = ARP_IP;
    msg->macaddr_len = 6;
    msg->ipaddr_len = 4;
    msg->op = ARP_REQUEST;
    memcpy(msg->macaddr_src, macaddr, 6);
    memcpy(msg->ipaddr_src, ipaddr, 4);
    memcpy(msg->macaddr_dst, macnull, 6);
    memcpy(msg->ipaddr_dst, ip_addr, 4);
    memcpy(frame->addr_dest, macbroadcast, 6);
    memcpy(frame->addr_src, macaddr, 6);
    frame->type = ETH_ARP;
    enc28j60_packetSend((void *)frame, sizeof(arp_msg_ptr) + sizeof(enc28j60_frame_ptr));
    return 1;
}