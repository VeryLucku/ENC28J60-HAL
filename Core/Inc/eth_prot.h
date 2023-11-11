#ifndef ETH_PROT_H_
#define ETH_PROT_H_

#include "net_structs.h"
#include "net.h"

void eth_read(enc28j60_frame_ptr *frame, uint16_t len);
void eth_send(enc28j60_frame_ptr *frame, uint16_t len);

#endif // ETH_PROT_H_