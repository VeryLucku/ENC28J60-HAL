#ifndef NET_STRUCTS_H_
#define NET_STRUCTS_H_

#include <stdint.h>

#define USART_BUF_LEN 25

typedef enum
{
    DEFAULT,
    UDP_MOD
} checksum_type;

typedef enum
{
    NO_SEND,
    ARP_SEND,
    TRY_UDP_SEND,
    UDP_SEND
} send_type;

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

typedef struct ip_pkt
{
    uint8_t verlen;        // версия протокола и длина заголовка
    uint8_t ts;            // тип севриса
    uint16_t len;          // длина
    uint16_t id;           // идентификатор пакета
    uint16_t fl_frg_of;    // флаги и смещение фрагмента
    uint8_t ttl;           // время жизни
    uint8_t prt;           // тип протокола
    uint16_t cs;           // контрольная сумма заголовка
    uint8_t ipaddr_src[4]; // IP-адрес отправителя
    uint8_t ipaddr_dst[4]; // IP-адрес получателя
    uint8_t data[];        // данные
} ip_pkt_ptr;

typedef struct icmp_pkt
{
    uint8_t msg_tp; // тип сервиса
    uint8_t msg_cd; // код сообщения
    uint16_t cs;    // контрольная сумма заголовка
    uint16_t id;    // идентификатор пакета
    uint16_t num;   // номер пакета
    uint8_t data[]; // данные
} icmp_pkt_ptr;

typedef struct udp_pkt {
    uint16_t port_src;
    uint16_t port_dst;
    uint16_t len;
    uint16_t cs;
    uint8_t data[];
} udp_pkt_ptr;

typedef struct USART_prop
{
    uint8_t usart_buf[USART_BUF_LEN];
    uint8_t usart_cnt;
    send_type send_type;
} USART_prop_ptr;

#endif // NET_STRUCTS_H_