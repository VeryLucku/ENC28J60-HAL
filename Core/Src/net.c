#include "net.h"

char str1[60] = {0};

typedef struct enc28j60_frame
{

    uint8_t addr_dest[6];

    uint8_t addr_src[6];

    uint16_t type;

    uint8_t data[];

} enc28j60_frame_ptr;

typedef struct arp_msg
{
    uint16_t net_tp;
    uint16_t proto_tp;
    uint8_t macaddr_len;
    uint8_t ipaddr_len;
    uint16_t op;
    uint8_t macaddr_src[6];
    uint8_t ipaddr_src[4];
    uint8_t macaddr_dst[6];
    uint8_t ipaddr_dst[4];
} arp_msg_ptr;

extern UART_HandleTypeDef huart2;

uint8_t net_buf[ENC28J60_MAXFRAME];
extern uint8_t macaddr[6];
uint8_t ipaddr[4] = IP_ADDR;

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
                    sprintf(str1, "requestrnmac_src %02X:%02X:%02X:%02X:%02X:%02Xrn",
                            msg->macaddr_src[0], msg->macaddr_src[1], msg->macaddr_src[2], msg->macaddr_src[3], msg->macaddr_src[4], msg->macaddr_src[5]);
                    HAL_UART_Transmit(&huart2, (uint8_t *)str1, strlen(str1), 0x1000);
                    sprintf(str1, "ip_src %d.%d.%d.%drn",
                            msg->ipaddr_src[0], msg->ipaddr_src[1], msg->ipaddr_src[2], msg->ipaddr_src[3]);
                    HAL_UART_Transmit(&huart2, (uint8_t *)str1, strlen(str1), 0x1000);
                    sprintf(str1, "nmac_dst %02X:%02X:%02X:%02X:%02X:%02Xrn",
                            msg->macaddr_dst[0], msg->macaddr_dst[1], msg->macaddr_dst[2], msg->macaddr_dst[3], msg->macaddr_dst[4], msg->macaddr_dst[5]);
                    HAL_UART_Transmit(&huart2, (uint8_t *)str1, strlen(str1), 0x1000);
                    sprintf(str1, "ip_dst %d.%d.%d.%drn",
                            msg->ipaddr_dst[0], msg->ipaddr_dst[1], msg->ipaddr_dst[2], msg->ipaddr_dst[3]);
                    HAL_UART_Transmit(&huart2, (uint8_t *)str1, strlen(str1), 0x1000);

                    res = 1;
                }
            }
        }
        return res;
    }
}

void eth_send(enc28j60_frame_ptr *frame, uint16_t len)
{
    memcpy(frame->addr_dest, frame->addr_src, 6);
    memcpy(frame->addr_src, macaddr, 6);
    enc28j60_packetSend((void *)frame, len + sizeof(enc28j60_frame_ptr));
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

void eth_read(enc28j60_frame_ptr *frame, uint16_t len)
{
    if (len >= sizeof(enc28j60_frame_ptr))
    {
        sprintf(str1, "%02X:%02X:%02X:%02X:%02X:%02X-%02X:%02X:%02X:%02X:%02X:%02X; %d; %04Xrn",

                frame->addr_src[0], frame->addr_src[1], frame->addr_src[2], frame->addr_src[3], frame->addr_src[4], frame->addr_src[5],

                frame->addr_dest[0], frame->addr_dest[1], frame->addr_dest[2], frame->addr_dest[3], frame->addr_dest[4], frame->addr_dest[5], len, be16toword(frame->type));
        if (frame->type == ETH_ARP)
        {
            HAL_UART_Transmit(&huart2, (uint8_t *)str1, strlen(str1), 0x1000);
            if (arp_read(frame, len - sizeof(enc28j60_frame_ptr)))
            {
                arp_send(frame);
            }
        }
    }
}

void net_pool()
{
    uint16_t len;

    enc28j60_frame_ptr *frame = (void *)net_buf;

    while ((len = enc28j60_packetReceive(net_buf, sizeof(net_buf))) > 0)
    {
        eth_read(frame, len);
    }
}

void net_init()
{
    enc28j60_ini();
}