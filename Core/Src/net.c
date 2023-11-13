#include "net.h"

void ip_extract(char *ip_str, uint8_t len, uint8_t *ipextr);

char str1[60] = {0};
extern char str[20];
USART_prop_ptr usartprop;

extern UART_HandleTypeDef huart2;

uint8_t net_buf[ENC28J60_MAXFRAME];
extern uint8_t macaddr[6];
uint8_t ipaddr[4] = IP_ADDR;

void net_pool()
{
    uint16_t len;

    enc28j60_frame_ptr *frame = (void *)net_buf;

    while ((len = enc28j60_packetReceive(net_buf, sizeof(net_buf))) > 0)
    {
        eth_read(frame, len);
    }
}

void net_cmd()
{
    static uint8_t ip[4] = {0};
    static uint16_t port = 0;

    // if ip address is sent from pc
    if (usartprop.send_type == ARP_SEND)
    {
        ip_extract((char *)usartprop.usart_buf, usartprop.usart_cnt, ip);
        arp_request(ip);
        usartprop.send_type = 0;
        usartprop.usart_cnt = 0;
    }
    else if (usartprop.send_type == TRY_UDP_SEND)
    {
        ip_extract((char *)usartprop.usart_buf, usartprop.usart_cnt, ip);
        usartprop.send_type = UDP_SEND;
        usartprop.usart_cnt = 0;
        // recognize mac address
        arp_request(ip);
    }
    else if (usartprop.send_type == UDP_SEND)
    {
        port = port_extract((char *)usartprop.usart_buf, usartprop.usart_cnt);
        udp_send(ip, port);
        usartprop.send_type = RESET;
    }
}

void net_init()
{
    usartprop.usart_buf[0] = 0;
    usartprop.usart_cnt = 0;
    usartprop.send_type = 0;
    enc28j60_init();
}

uint16_t port_extract(char *ip_str, uint8_t len)
{
    uint16_t port = 0;
    int ch1 = ':';
    char *ss1;
    uint8_t offset = 0;
    ss1 = strchr(ip_str, ch1);
    offset = ss1 - ip_str + 1;
    ip_str += offset;
    port = atoi(ip_str);

    return port;
}

void ip_extract(char *ip_str, uint8_t len, uint8_t *ipextr)
{
    uint8_t offset = 0;
    uint8_t i;
    char ss2[5] = {0};
    char *ss1;
    int ch1 = '.';
    int ch2 = ':';

    for (i = 0; i < 3; ++i)
    {
        ss1 = strchr(ip_str, ch1);

        offset = ss1 - ip_str + 1;
        ss2[offset] = 0;
        ipextr[i] = atoi(ss2);
    }

    ss1 = strchr(ip_str, ch2);
    if (ss1 != NULL)
    {
        offset = ss1 - ip_str + 1;
        strncpy(ss2, ip_str, offset);
        ss2[offset];
        ipextr[3] = atoi(ss2);
        return;
    }
    strncpy(ss2, ip_str, len);
    ss2[len] = 0;
    ipextr[3] = atoi(ss2);
}

void UART2_RxCpltCallback()
{
    uint8_t b;
    b = str[0];

    if (usartprop.usart_cnt > USART_BUF_LEN)
    {
        usartprop.usart_cnt = 0;
    }
    else if (b == 'a')
    {
        usartprop.send_type = ARP_SEND;
        net_cmd();
    }
    else if (b == 'u')
    {
        usartprop.send_type = TRY_UDP_SEND;
        net_cmd();
    }
    else
    {
        usartprop.usart_buf[usartprop.usart_cnt++] = b;
    }

    HAL_UART_Receive_IT(&huart2, (uint8_t *)str, 1);
}
