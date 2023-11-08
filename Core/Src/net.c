#include "net.h"

// data linj level
void eth_read(enc28j60_frame_ptr *frame, uint16_t len);
void eth_send(enc28j60_frame_ptr *frame, uint16_t len);
// network level
uint8_t arp_read(enc28j60_frame_ptr *frame, uint16_t len);
void arp_send(enc28j60_frame_ptr *frame);

uint8_t ip_read(enc28j60_frame_ptr *frame, uint16_t len);
uint8_t ip_send(enc28j60_frame_ptr *frame, uint16_t len);
// between network and transport level
uint8_t icmp_read(enc28j60_frame_ptr *frame, uint16_t len);

char str1[60] = {0};

extern UART_HandleTypeDef huart2;

uint8_t net_buf[ENC28J60_MAXFRAME];
extern uint8_t macaddr[6];
uint8_t ipaddr[4] = IP_ADDR;



uint8_t icmp_read(enc28j60_frame_ptr *frame, uint16_t len)
{
    uint8_t res = 0;
    ip_pkt_ptr *ip_pkt = (void *)frame->data;

    icmp_pkt_ptr *icmp_pkt = (void *)ip_pkt->data;

    if ((len >= sizeof(icmp_pkt_ptr)) && (icmp_pkt->msg_tp == ICMP_REQ))
    {
        icmp_pkt->msg_tp = ICMP_REPLY;
        icmp_pkt->cs = 0;
        icmp_pkt->cs = eth_checksum((void *)icmp_pkt, len);
        ip_send(frame, len+sizeof(ip_pkt_ptr));
    }

    return res;
}

uint8_t ip_read(enc28j60_frame_ptr *frame, uint16_t len)
{
    uint8_t res = 0;
    ip_pkt_ptr *ip_pkt = (void *)(frame->data);

    if ((ip_pkt->verlen == 0x45) && (!memcmp(ip_pkt->ipaddr_dst, ipaddr, 4)))
    {
        len = be16toword(ip_pkt->len) - sizeof(ip_pkt_ptr);

        if (ip_pkt->prt == IP_ICMP)
        {
            icmp_read(frame, len);
        }
        else if (ip_pkt->prt == IP_TCP)
        {
        }
        else if (ip_pkt->prt == IP_UDP)
        {
        }
    }

    return res;
}

uint8_t ip_send(enc28j60_frame_ptr *frame, uint16_t len)
{

    uint8_t res = 0;
    ip_pkt_ptr *ip_pkt = (void*)frame->data;
    ip_pkt->len = be16toword(len);
    ip_pkt->fl_frg_of = 0;
    ip_pkt->ttl=128;
    ip_pkt->cs=0;
    memcpy(ip_pkt->ipaddr_dst, ip_pkt->ipaddr_src,4);
    memcpy(ip_pkt->ipaddr_src, ipaddr,4);

    ip_pkt->cs = eth_checksum((void *)ip_pkt, sizeof(ip_pkt_ptr));

    eth_send(frame, len);

    return res;
}

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
        return res;
    }
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

        if (frame->type == ETH_IP)
        {
            ip_read(frame, len - sizeof(ip_pkt_ptr));
        }
    }
}

void eth_send(enc28j60_frame_ptr *frame, uint16_t len)
{
    memcpy(frame->addr_dest, frame->addr_src, 6);
    memcpy(frame->addr_src, macaddr, 6);
    enc28j60_packetSend((void *)frame, len + sizeof(enc28j60_frame_ptr));
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
    enc28j60_init();
}