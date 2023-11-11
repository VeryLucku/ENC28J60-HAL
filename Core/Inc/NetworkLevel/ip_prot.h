#ifndef IP_PROT_H_
#define IP_PROT_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "enc28j60.h"
#include "net_structs.h"
#include "net.h"
#include "TransportLevel/icmp_prot.h"
#include "DataLinkLevel/eth_prot.h"

uint8_t ip_read(enc28j60_frame_ptr *frame, uint16_t len);
uint8_t ip_send(enc28j60_frame_ptr *frame, uint16_t len);

#endif // IP_PROT_H_