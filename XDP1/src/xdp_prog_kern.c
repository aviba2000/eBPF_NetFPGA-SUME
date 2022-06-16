#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/ipv6.h>

#include <stdio.h>
#include <stddef.h>
#include <endian.h>

#include "xdp_prog_kern.h"


void copy_tmp(int pos, uint8_t data[MAX_PACKET_LEN], uint8_t data_tmp[MAX_PACKET_LEN], int size) {
	int i;
	for (i = 0; i < MAX_PACKET_LEN; i++) {
		if (i >= pos && i < size) {
			data_tmp[i] = data[i];
		}
	}
}

void copy_array_2(uint8_t data[MAX_PACKET_LEN], uint8_t data_tmp[MAX_PACKET_LEN], uint8_t data_tmp2[MAX_PACKET_LEN], unsigned int size){
	int i;
	uint8_t aux;
	for (i = 0; i < MAX_PACKET_LEN; i++) {
		if (i < size) {
			aux = data[i];
			data_tmp[i] = aux;
			data_tmp2[i] = aux;
		}
	}
}

int parsing_task(const uint8_t data_tmp[MAX_PACKET_LEN], const uint8_t data_tmp2[MAX_PACKET_LEN], unsigned int size, long rxcnt[256],
		union ethhdr_byte_data *eth, union vlanhdr_byte_data *vlan1, union vlanhdr_byte_data *vlan2,
		union iphdr_byte_data *iph, union ipv6hdr_byte_data *ipv6h) {
	int size_pipelining, pos;
	pos = 0;
	size_pipelining = size;

	int nh_type = parse_ethhdr(&pos, data_tmp, eth, size);
	if (nh_type == -1) return -1;

	if (!(nh_type == htobe16(ETH_P_8021Q) || nh_type == htobe16(ETH_P_8021AD))) {
		size_pipelining = -1;
	}

	int nh_type_vlan = parse_vlanhdr(&pos, data_tmp, vlan1, size_pipelining);
	if (nh_type_vlan == -1) return -1;


	if (!(nh_type_vlan == htobe16(ETH_P_8021Q) || nh_type_vlan == htobe16(ETH_P_8021AD))) {
		size_pipelining = -1;
	}
	int nh_type_vlan_2 = parse_vlanhdr(&pos, data_tmp, vlan2, size_pipelining);
	if (nh_type_vlan == -1) return -1;

	int nh_type2 = 0;

	int is_IPv4 = (nh_type == htobe16(ETH_P_IP) || nh_type_vlan == htobe16(ETH_P_IP) || nh_type_vlan_2 == htobe16(ETH_P_IP));
	int is_IPv6 = (nh_type == htobe16(ETH_P_IPV6) || nh_type_vlan == htobe16(ETH_P_IPV6) || nh_type_vlan_2 == htobe16(ETH_P_IP));

	if (is_IPv4) {
		nh_type2 = parse_iphdr(&pos, data_tmp2, iph, size);
		if (nh_type2 == -1) return -1;
	} else if (is_IPv6) {
		nh_type2 = parse_ipv6hdr(&pos, data_tmp2, ipv6h, size);
		if (nh_type2 == -1) return -1;
	}


	if (nh_type2 < 256 && nh_type >= 0) rxcnt[nh_type2]++;

	return 0;
}


// https://github.com/torvalds/linux/blob/master/samples/bpf/xdp1_kern.c
inline int xdp_prog1_OPTIMIZED(uint8_t data[MAX_PACKET_LEN], unsigned int size, long rxcnt[256]) {
	// #pragma HLS pipeline // It is already in the top function!
	#pragma HLS INTERFACE mode=ap_fifo port=data

	uint8_t data_tmp[MAX_PACKET_LEN];
	uint8_t data_tmp2[MAX_PACKET_LEN];

	#pragma HLS array_partition variable=data complete
	#pragma HLS array_partition variable=data_tmp complete
	#pragma HLS array_partition variable=data_tmp2 complete


	copy_array_2(data, data_tmp, data_tmp2, size);

	int action = XDP_DROP;

	union ethhdr_byte_data eth;
	union vlanhdr_byte_data vlan1;
	union vlanhdr_byte_data vlan2;
	union iphdr_byte_data iph;
	union ipv6hdr_byte_data ipv6h;

	int err = parsing_task(data_tmp, data_tmp2, size, rxcnt, &eth, &vlan1, &vlan2, &iph, &ipv6h);
	if (err < 0) return -1;

	return action;
}



inline int xdp_prog1_NON_OPTIMIZED(uint8_t data[MAX_PACKET_LEN], unsigned int size, long rxcnt[256]) {
	int action = XDP_DROP;

	union ethhdr_byte_data eth;
	union vlanhdr_byte_data vlan1;
	union vlanhdr_byte_data vlan2;
	union iphdr_byte_data iph;
	union ipv6hdr_byte_data ipv6h;

	int err = parsing_task(data, data, size, rxcnt, &eth, &vlan1, &vlan2, &iph, &ipv6h);
	if (err < 0) return -1;

	return action;
}


int xdp_prog1(uint8_t data[MAX_PACKET_LEN], unsigned int size, long rxcnt[256]) {
	#if OPTIMIZATION
	#pragma HLS pipeline // Otherwise there are one/two cycles of overhead in the optimized version
	#endif
	if (!OPTIMIZATION) return xdp_prog1_NON_OPTIMIZED(data, size, rxcnt);
	else return xdp_prog1_OPTIMIZED(data, size, rxcnt);
}
