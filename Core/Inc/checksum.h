#ifndef CHECKSUM_H_
#define CHECKSUM_H_

#include <stdint.h>

uint16_t eth_checksum(uint8_t *ptr, uint16_t len);

#endif // CHECKSUM_H_