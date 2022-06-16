#ifndef XDP2_FUNC_
#define XDP2_FUNC_

#include <stdint.h>
#include <linux/ipv6.h>

#include "../../00_Common_Libraries/src/parsing_helpers.h"
#include "../../00_Common_Libraries/src/writing_helpers.h"

#define IPPROTO_UDP 17

int xdp2(const uint8_t data_in[ETH_TOTAL_LEN], uint8_t data_out[ETH_TOTAL_LEN], unsigned int size_in, unsigned int *size_out, long rxcnt[256]);

#endif
