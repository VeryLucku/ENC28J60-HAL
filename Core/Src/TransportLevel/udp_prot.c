#include "TransportLevel/udp_prot.h"

extern UART_HandleTypeDef huart2;

uint8_t udp_read(enc28j60_frame_ptr *frame, uint16_t len)
{
    uint8_t res = 0;

    ip_pkt_ptr *ip_pkt = (void *)(frame->data);

    udp_pkt_ptr *udp_pkt = (void *)(ip_pkt->data);

    udp_reply(frame, len);

    return res;
}

uint8_t udp_reply(enc28j60_frame_ptr *frame, uint16_t len)
{
    uint8_t res = 0;

    uint16_t port;
    ip_pkt_ptr *ip_pkt = (void *)(frame->data);
    udp_pkt_ptr *udp_pkt = (void *)(ip_pkt->data);
    port = udp_pkt->port_dst;
    udp_pkt->port_dst = udp_pkt->port_src;
    udp_pkt->port_src = port;

    strcpy((char *)udp_pkt->data, "Hello from STM (UDP)");

    len = strlen((char *)udp_pkt->data + sizeof(udp_pkt_ptr));
    udp_pkt->len = be16toword(len);

    udp_pkt->cs = 0;
    udp_pkt->cs = eth_checksum((uint8_t *)udp_pkt - 8, len + 8, UDP_MOD);
    ip_send(frame, len+sizeof(ip_pkt_ptr));

    return res;
}