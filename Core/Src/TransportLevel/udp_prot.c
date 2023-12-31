#include "TransportLevel/udp_prot.h"

extern UART_HandleTypeDef huart2;
extern uint8_t net_buf[ENC28J60_MAXFRAME];

uint8_t udp_read(enc28j60_frame_ptr *frame, uint16_t len)
{

    udp_reply(frame, len);

    return 0;
}

uint8_t udp_send(uint8_t *ip_addr, uint16_t port)
{
    uint8_t res = 0;
    uint16_t len;
    enc28j60_frame_ptr *frame = (void *)net_buf;
    ip_pkt_ptr *ip_pkt = (void *)(frame->data);
    udp_pkt_ptr *udp_pkt = (void *)(ip_pkt->data);

    udp_pkt->port_dst = be16toword(port);
    udp_pkt->port_src = be16toword(LOCAL_PORT);

    strcpy((char *)udp_pkt->data, "UDP data");
    len = strlen((char *)udp_pkt->data)+sizeof(udp_pkt_ptr);

    udp_pkt->len = be16toword(len);
    udp_pkt->cs = 0;
    udp_pkt->cs = eth_checksum((uint8_t *)udp_pkt-8, len+8, UDP_MOD);

    memcpy(ip_pkt->ipaddr_src, ip_addr, 4);
    ip_pkt->prt = IP_UDP;
    ip_pkt->id = 0;
    ip_pkt->ts = 0;
    ip_pkt->verlen = 0x45;

    frame->type = ETH_IP;
    ip_send(frame, len+sizeof(ip_pkt_ptr));

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

    memcpy(frame->addr_dest, frame->addr_src, 6);
    ip_send(frame, len + sizeof(ip_pkt_ptr));

    return res;
}