#include "TransportLevel/udp_prot.h"

extern UART_HandleTypeDef huart2;

uint8_t udp_read(enc28j60_frame_ptr *frame, uint16_t len) {
    uint8_t res = 0;

    ip_pkt_ptr *ip_pkt = (void*)(frame->data);
    

    return res;
}