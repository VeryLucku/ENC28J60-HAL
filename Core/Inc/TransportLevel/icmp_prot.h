#ifndef ICMP_PROT_H_
#define ICMP_PROT_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "enc28j60.h"
#include "net_structs.h"
#include "checksum.h"
#include "net.h"

uint8_t icmp_read(enc28j60_frame_ptr *frame, uint16_t len);

#endif // ICMP_PROT_H_