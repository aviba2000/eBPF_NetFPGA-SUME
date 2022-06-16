#ifndef __FAST_HELPERS__
#define __FAST_HELPERS__

#include "../def/DataTypes.h"

// The get_value or write_value functions HAVE to be inlined
inline unsigned short get_value16(uint8_t *ppos, unsigned int offset);
inline unsigned int get_value32(uint8_t *ppos, unsigned int offset);
inline unsigned long get_value64(uint8_t *ppos, unsigned int offset);

inline void write_value16(uint8_t *ppos, unsigned int offset, unsigned short new_val);
inline void write_value32(uint8_t *ppos, unsigned int offset, unsigned int new_val);
inline void write_value64(uint8_t *ppos, unsigned int offset, unsigned long new_val);

inline unsigned short parse_Eth_fast(uint8_t **ppos, uint8_t *data_end);
inline unsigned short parse_VLAN_fast(uint8_t **ppos, uint8_t *data_end);

inline uint8_t parse_IPv4_fast(uint8_t **ppos, uint8_t *data_end);
inline uint8_t parse_IPv6_fast(uint8_t **ppos, uint8_t *data_end);
#endif
