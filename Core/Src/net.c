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
    uint8_t ip[4] = {0};

    enc28j60_frame_ptr *frame = (void *)net_buf;

    while ((len = enc28j60_packetReceive(net_buf, sizeof(net_buf))) > 0)
    {
        eth_read(frame, len);
    }

    
}

void net_cmd()
{
    uint8_t ip[4] = {0};
    
    // if ip address is sent from pc
    if (usartprop.is_ip == 1)
    {
        ip_extract((char *)usartprop.usart_buf, usartprop.usart_cnt, ip);
        arp_request(ip);
        usartprop.is_ip = 0;
        usartprop.usart_cnt = 0;
    }
}

void net_init()
{
    usartprop.usart_buf[0] = 0;
    usartprop.usart_cnt = 0;
    usartprop.is_ip = 0;
    enc28j60_init();
}

void ip_extract(char *ip_str, uint8_t len, uint8_t *ipextr)
{
    uint8_t offset = 0;
    uint8_t i;
    char ss2[5] = {0};
    char *ss1;
    int ch = '.';

    for (i = 0; i < 3; ++i)
    {
        ss1 = strchr(ip_str, ch);

        offset = ss1 - ip_str + 1;
        ss2[offset] = 0;
        ipextr[i] = atoi(ss2);
    }

    strncpy(ss2, ip_str, len);
    ss2[len] = 0;
    ipextr[3] = atoi(ss2);
}

void UART2_RxCpltCallback()
{
    uint8_t b;
    b = str[0];

    if (usartprop.usart_cnt > 20)
    {
        usartprop.usart_cnt = 0;
    }
    else if (b == 'a')
    {
        usartprop.is_ip = 1;
        net_cmd();
    }
    else
    {
        usartprop.usart_buf[usartprop.usart_cnt++] = b;
    }

    HAL_UART_Receive_IT(&huart2, (uint8_t *)str, 1);
}
