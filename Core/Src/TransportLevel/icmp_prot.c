#include "TransportLevel/icmp_prot.h"

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
        ip_send(frame, len + sizeof(ip_pkt_ptr));
    }

    return res;
}