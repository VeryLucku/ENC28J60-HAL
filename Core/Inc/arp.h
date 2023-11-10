#ifndef ARP_H_
#define ARP_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "enc28j60.h"
#include "net.h"

uint8_t arp_request(uint8_t *ip_addr);

void arp_table_fill(enc28j60_frame_ptr *frame);

typedef struct arp_record {
    uint8_t ipaddr[4];
    uint8_t macaddr[6];
    // time when record was made
    uint32_t sec;
} arp_record_ptr;

#endif // ARP_H_