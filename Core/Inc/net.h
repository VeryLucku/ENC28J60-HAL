#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "enc28j60.h"

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

void net_init();