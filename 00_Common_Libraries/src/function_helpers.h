#ifndef __FUNCTION_HELPERS__
#define __FUNCTION_HELPERS__

#include "../def/DataTypes.h"

uint16_t IPV4Checksum(uint16_t *pkt, uint16_t hlen, uint16_t init);

uint16_t Checksum(uint16_t *ptr, int len, unsigned int acc);

uint16_t Checksum_8bits_optimized(uint8_t *ptr, int len, unsigned int acc);

#endif
