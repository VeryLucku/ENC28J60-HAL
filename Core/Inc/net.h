#ifndef NET_H_
#define NET_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "enc28j60.h"
#include "checksum.h"
#include "net_structs.h"
#include "NetworkLevel/arp_prot.h"
#include "DataLinkLevel/eth_prot.h"


#define be16toword(a) ((((a) >> 8) & 0xff) | (((a) << 8) & 0xff00))

#define ETH_ARP be16toword(0x0806)
#define ETH_IP be16toword(0x0800)

#define ARP_ETH be16toword(0x0001)
#define ARP_IP be16toword(0x0800)
#define ARP_REQUEST be16toword(1)
#define ARP_REPLY be16toword(2)

#define IP_ADDR          \
    {                    \
        192, 168, 1, 197 \
    }
#define MAC_BROADCAST {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#define MAC_NULL {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define IP_ICMP 1
#define IP_TCP 6
#define IP_UDP 17

#define ICMP_REQ 8
#define ICMP_REPLY 0

void net_init();
void net_pool();

void UART2_RxCpltCallback();



#endif // NET_H_