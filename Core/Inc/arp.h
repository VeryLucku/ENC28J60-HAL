#ifndef ARP_H_
#define ARP_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "enc28j60.h"
#include "net.h"

uint8_t arp_request(uint8_t *ip_addr);

#endif // ARP_H_