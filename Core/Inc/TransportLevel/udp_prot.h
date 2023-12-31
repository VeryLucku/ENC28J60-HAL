#ifndef UDP_PROT_H_
#define UDP_PROT_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "enc28j60.h"
#include "net_structs.h"
#include "checksum.h"
#include "net.h"

#define LOCAL_PORT 1234

uint8_t udp_read(enc28j60_frame_ptr *frame, uint16_t len);
uint8_t udp_send(uint8_t *ip_addr, uint16_t port);
uint8_t udp_reply(enc28j60_frame_ptr *frame, uint16_t len);

#endif // UDP_PROT_H_