#include "fast_helpers.h"
#include <stddef.h>

inline unsigned short get_value16(uint8_t *ppos, unsigned int offset) {
	union pos_val value;
	value.pos_8 = &ppos[offset];
	return *(value.pos_16);
}

inline unsigned int get_value32(uint8_t *ppos, unsigned int offset) {
	union pos_val value;
	value.pos_8 = &ppos[offset];
	return *(value.pos_32);
}

inline unsigned long get_value64(uint8_t *ppos, unsigned int offset) {
	union pos_val value;
	value.pos_8 = &ppos[offset];
	return *(value.pos_64);
}

inline void write_value16(uint8_t *ppos, unsigned int offset, unsigned short new_val) {
	union pos_val value;
	value.pos_8 = &ppos[offset];
	*(value.pos_16) = new_val;
}

inline void write_value32(uint8_t *ppos, unsigned int offset, unsigned int new_val) {
	union pos_val value;
	value.pos_8 = &ppos[offset];
	*(value.pos_32) = new_val;
}

inline void write_value64(uint8_t *ppos, unsigned int offset, unsigned long new_val) {
	union pos_val value;
	value.pos_8 = &ppos[offset];
	*(value.pos_64) = new_val;
}

inline unsigned short parse_Eth_fast(uint8_t **ppos, uint8_t *data_end) {
	if (*ppos + ETH_HLEN > data_end) return -1;
	unsigned short nh_type = get_value16(*ppos, offsetof(struct ethhdr, h_proto));
	*ppos += ETH_HLEN;
	return nh_type;
}

inline unsigned short parse_VLAN_fast(uint8_t **ppos, uint8_t *data_end) {
	unsigned int VLAN_HLEN = sizeof(struct vlan_hdr);
	if (*ppos + VLAN_HLEN >= data_end) return -1;
	unsigned short nh_type = get_value16(*ppos, offsetof(struct vlan_hdr, h_vlan_encapsulated_proto));
	*ppos += VLAN_HLEN;
	return nh_type;
}


inline uint8_t parse_IPv4_fast(uint8_t **ppos, uint8_t *data_end) {
	if (*ppos + IPv4_BASE_LEN >= data_end) return -1;
	unsigned int ip_hlen = (unsigned int) (*ppos[0] & 0xF)*4; // Little endian
	if(ip_hlen < sizeof(struct iphdr)) return -1;
	if (*ppos + ip_hlen > data_end) return -1;
	uint8_t nh_type_8 = (*ppos)[offsetof(struct iphdr, protocol)];
	*ppos += IPv4_BASE_LEN;
	return nh_type_8;
}

inline uint8_t parse_IPv6_fast(uint8_t **ppos, uint8_t *data_end) {
	unsigned int IPv6_HLEN = sizeof(struct ipv6hdr);
	if (*ppos + IPv6_HLEN >= data_end) return -1;
	uint8_t nh_type_8 = (*ppos)[offsetof(struct ipv6hdr, nexthdr)];
	*ppos += IPv6_HLEN;
	return nh_type_8;
}
