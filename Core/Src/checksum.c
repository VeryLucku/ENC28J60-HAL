#include "checksum.h"
#include "net.h"

uint16_t eth_checksum(uint8_t *ptr, uint16_t len)
{
    uint32_t sum = 0;

    while (len > 1)
    {
        sum += (uint16_t)(((uint32_t)*ptr << 8) | *(ptr + 1));
        ptr += 2;
        len -= 2;
    }

    if (len)
        sum += ((uint32_t)*ptr) << 8;

    while (sum >> 16)
        sum = (uint16_t)sum + (sum >> 16);

    return ~be16toword((uint16_t)sum);
}