#ifndef __DATATYPES__
#define __DATATYPES__

#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/icmp.h>
#include <stdint.h>
#include <stdio.h>
#include <endian.h>

#define OPTIMIZATION 1 // 0 -> NO, 1 -> YES

#define MAX_PACKET_LEN 300

#define ETH_TOTAL_LEN 1518
#define VLAN_HDR_LEN 4
#define IPv4_BASE_LEN sizeof(struct iphdr)
#define IPv6_HDR_LEN 40
#define ICMP_HDR_LEN 8

#ifndef __SYNTHESIS__
#	define PRINT_DEBUG(str, args...) printf(str, ##args)
#else
#	define PRINT_DEBUG(str, args...)
#endif

/*
	#ifndef __SYNTHESIS__
		printf("This is the proper way to debug\n");
	#endif

	Is equal to: PRINT_DEBUG("This is the proper way to debug\n");
*/

struct vlan_hdr {
	__be16	h_vlan_TCI;
	__be16	h_vlan_encapsulated_proto;
};

union ethhdr_byte_data {
	struct ethhdr eth;
	uint8_t bytes[ETH_HLEN];
};

union vlanhdr_byte_data {
	struct vlan_hdr vlan;
	uint8_t bytes[VLAN_HDR_LEN];
};

union iphdr_byte_data {
	struct iphdr iph;
	uint8_t bytes[60]; // Maximum IPv4 header size (it is variable) = 60 Bytes
	uint16_t bytes16[30];
};

union ipv6hdr_byte_data {
	struct ipv6hdr ipv6h;
	uint8_t bytes[IPv6_HDR_LEN];
};

union icmphdr_byte_data {
	struct icmphdr icmph;
	uint8_t bytes[ICMP_HDR_LEN];
	uint16_t bytes16[ICMP_HDR_LEN/2];
};


union pos_val {
	uint8_t *pos_8;
	unsigned short *pos_16;
	unsigned int *pos_32;
	unsigned long *pos_64;
};

#endif
