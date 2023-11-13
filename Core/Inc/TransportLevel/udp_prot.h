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

uint8_t udp_read(enc28j60_frame_ptr *frame, uint16_t len);

#endif // UDP_PROT_H_