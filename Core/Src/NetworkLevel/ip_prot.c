#include "NetworkLevel/ip_prot.h"

extern uint8_t ipaddr[4];

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
            udp_read(frame, len);
        }
    }

    return res;
}

uint8_t ip_send(enc28j60_frame_ptr *frame, uint16_t len)
{

    uint8_t res = 0;
    ip_pkt_ptr *ip_pkt = (void *)frame->data;
    ip_pkt->len = be16toword(len);
    ip_pkt->fl_frg_of = 0;
    ip_pkt->ttl = 128;
    ip_pkt->cs = 0;
    memcpy(ip_pkt->ipaddr_dst, ip_pkt->ipaddr_src, 4);
    memcpy(ip_pkt->ipaddr_src, ipaddr, 4);

    ip_pkt->cs = eth_checksum((void *)ip_pkt, sizeof(ip_pkt_ptr), DEFAULT);

    eth_send(frame, len);

    return res;
}