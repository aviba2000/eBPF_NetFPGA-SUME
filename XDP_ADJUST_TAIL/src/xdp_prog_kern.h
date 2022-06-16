#ifndef XDP_ADJ_TAIL_FUNC_
#define XDP_ADJ_TAIL_FUNC_

#include <stdint.h>
#include <linux/ipv6.h>
#include <endian.h>

#include "../../00_Common_Libraries/src/parsing_helpers.h"
#include "../../00_Common_Libraries/src/writing_helpers.h"
#include "../../00_Common_Libraries/src/function_helpers.h"

#define IPPROTO_UDP 17
#define IPPROTO_ICMP 1

#define DEFAULT_TTL 64
#define MAX_PCKT_SIZE 98
#define ICMP_TOOBIG_SIZE 98
#define ICMP_TOOBIG_PAYLOAD_SIZE 92

#define max(a,b) (((a)>(b))?(a):(b))

static __u32 max_pcktsz = MAX_PCKT_SIZE;

int xdp_prog1(uint8_t data[ETH_TOTAL_LEN], uint8_t data_out[ETH_TOTAL_LEN], unsigned int in_size, unsigned int *out_size, long rxcnt[256]);

#endif
