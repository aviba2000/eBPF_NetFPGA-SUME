#ifndef XDP1_FUNC_
#define XDP1_FUNC_

#include <stdint.h>
#include <linux/ipv6.h>

#include "../../00_Common_Libraries/src/parsing_helpers.h"
#include "../../00_Common_Libraries/src/writing_helpers.h"

int xdp_prog1(uint8_t data[ETH_TOTAL_LEN], unsigned int size, long rxcnt[256]);

#endif
