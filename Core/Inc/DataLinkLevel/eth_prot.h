#ifndef ETH_PROT_H_
#define ETH_PROT_H_

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "enc28j60.h"
#include "net_structs.h"
#include "net.h"
#include "NetworkLevel/ip_prot.h"

void eth_read(enc28j60_frame_ptr *frame, uint16_t len);
void eth_send(enc28j60_frame_ptr *frame, uint16_t len);

#endif // ETH_PROT_H_