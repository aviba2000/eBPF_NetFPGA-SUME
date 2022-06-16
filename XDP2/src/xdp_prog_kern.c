#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/ipv6.h>

#include <stdlib.h> // malloc(...)
#include <stdio.h>
#include <stddef.h>
#include <endian.h>

#include "xdp_prog_kern.h"


void swap_src_dst_mac_1(union ethhdr_byte_data *eth) {
	unsigned char dst[6];

	dst[0] = eth->eth.h_dest[0];
	dst[1] = eth->eth.h_dest[1];
	dst[2] = eth->eth.h_dest[2];
	dst[3] = eth->eth.h_dest[3];
	dst[4] = eth->eth.h_dest[4];
	dst[5] = eth->eth.h_dest[5];

	eth->eth.h_dest[0] = eth->eth.h_source[0];
	eth->eth.h_dest[1] = eth->eth.h_source[1];
	eth->eth.h_dest[2] = eth->eth.h_source[2];
	eth->eth.h_dest[3] = eth->eth.h_source[3];
	eth->eth.h_dest[4] = eth->eth.h_source[4];
	eth->eth.h_dest[5] = eth->eth.h_source[5];
	eth->eth.h_source[0] = dst[0];
	eth->eth.h_source[1] = dst[1];
	eth->eth.h_source[2] = dst[2];
	eth->eth.h_source[3] = dst[3];
	eth->eth.h_source[4] = dst[4];
	eth->eth.h_source[5] = dst[5];
}

void swap_src_dst_mac(union ethhdr_byte_data *eth, int nh_type2, int *action) {
	unsigned char dst[6];

	if (nh_type2 == IPPROTO_UDP) {
		dst[0] = eth->eth.h_dest[0];
		dst[1] = eth->eth.h_dest[1];
		dst[2] = eth->eth.h_dest[2];
		dst[3] = eth->eth.h_dest[3];
		dst[4] = eth->eth.h_dest[4];
		dst[5] = eth->eth.h_dest[5];

		eth->eth.h_dest[0] = eth->eth.h_source[0];
		eth->eth.h_dest[1] = eth->eth.h_source[1];
		eth->eth.h_dest[2] = eth->eth.h_source[2];
		eth->eth.h_dest[3] = eth->eth.h_source[3];
		eth->eth.h_dest[4] = eth->eth.h_source[4];
		eth->eth.h_dest[5] = eth->eth.h_source[5];
		eth->eth.h_source[0] = dst[0];
		eth->eth.h_source[1] = dst[1];
		eth->eth.h_source[2] = dst[2];
		eth->eth.h_source[3] = dst[3];
		eth->eth.h_source[4] = dst[4];
		eth->eth.h_source[5] = dst[5];
		*action = XDP_TX;
	} else {
		*action = XDP_PASS;
	}
}

void copy_tmp(int pos, uint8_t data[MAX_PACKET_LEN], uint8_t data_tmp[MAX_PACKET_LEN], int size) {
	int i;
	for (i = 0; i < MAX_PACKET_LEN; i++) {
		if (i >= pos && i < size) {
			data_tmp[i] = data[i];
		}
	}
}

void copy_array(const uint8_t data[MAX_PACKET_LEN], uint8_t data_tmp[MAX_PACKET_LEN], uint8_t data_tmp2[MAX_PACKET_LEN], uint8_t data_tmp3[MAX_PACKET_LEN], unsigned int size){
	int i;
	uint8_t aux;
	for (i = 0; i < MAX_PACKET_LEN; i++) {
		if (i < size) {
			aux = data[i];
			data_tmp[i] = aux;
			data_tmp2[i] = aux;
			data_tmp3[i] = aux;
		}
	}
}

int parsing_task(const uint8_t data_tmp[MAX_PACKET_LEN], const uint8_t data_tmp2[MAX_PACKET_LEN], unsigned int size, long rxcnt[256],
		union ethhdr_byte_data *eth, union vlanhdr_byte_data *vlan1, union vlanhdr_byte_data *vlan2,
		union iphdr_byte_data *iph, union ipv6hdr_byte_data *ipv6h, int *num_vlan, int *is_IPv4, int *is_IPv6, int *action) {
	int size_pipelining, pos;
	pos = 0;
	size_pipelining = size;

	int nh_type = parse_ethhdr(&pos, data_tmp, eth, size);
	if (nh_type == -1) return -1;

	if (nh_type == htobe16(ETH_P_8021Q) || nh_type == htobe16(ETH_P_8021AD)) {
		*num_vlan = *num_vlan+1;
	} else {
		size_pipelining = -1;
	}

	int nh_type_vlan = parse_vlanhdr(&pos, data_tmp, vlan1, size_pipelining);
	if (nh_type_vlan == -1) return -1;

	if (nh_type_vlan == htobe16(ETH_P_8021Q) || nh_type_vlan == htobe16(ETH_P_8021AD)) {
		*num_vlan = *num_vlan+1;
	} else {
		size_pipelining = -1;
	}

	int nh_type_vlan_2 = parse_vlanhdr(&pos, data_tmp, vlan2, size_pipelining);
	if (nh_type_vlan == -1) return -1;

	int nh_type2 = 0;

	*is_IPv4 = (nh_type == htobe16(ETH_P_IP) || nh_type_vlan == htobe16(ETH_P_IP) || nh_type_vlan_2 == htobe16(ETH_P_IP));
	*is_IPv6 = (nh_type == htobe16(ETH_P_IPV6) || nh_type_vlan == htobe16(ETH_P_IPV6) || nh_type_vlan_2 == htobe16(ETH_P_IP));

	if (*is_IPv4) {
		nh_type2 = parse_iphdr(&pos, data_tmp2, iph, size);
		if (nh_type2 == -1) return -1;
	} else if (*is_IPv6) {
		nh_type2 = parse_ipv6hdr(&pos, data_tmp2, ipv6h, size);
		if (nh_type2 == -1) return -1;
	}

	if (nh_type2 < 256 && nh_type2 >= 0) rxcnt[nh_type2] = rxcnt[nh_type2]+1;

	swap_src_dst_mac(eth, nh_type2, action);

	return 0;
}

int writing_task(uint8_t data_out[MAX_PACKET_LEN], const uint8_t data_tmp3[MAX_PACKET_LEN], unsigned int size,
		const union ethhdr_byte_data *eth, const union vlanhdr_byte_data *vlan1, const union vlanhdr_byte_data *vlan2,
		const union iphdr_byte_data *iph, const union ipv6hdr_byte_data *ipv6h, int num_vlan, int is_IPv4, int is_IPv6) {
	int pos_pipelining;
	int pos = ETH_HLEN;
	write_Eth_hdr(pos, data_out, eth);

	pos += VLAN_HDR_LEN;
	if (num_vlan > 0) pos_pipelining = pos;
	else {
		pos -= VLAN_HDR_LEN;
		pos_pipelining = -1;
	}

	write_VLAN_hdr(pos_pipelining, data_out, vlan1);

	pos += VLAN_HDR_LEN;
	if (num_vlan > 1) pos_pipelining = pos;
	else {
		pos -= VLAN_HDR_LEN;
		pos_pipelining = -1;
	}

	write_VLAN_hdr(pos_pipelining, data_out, vlan2);

	if (is_IPv4) {
		pos += 20;
		write_IPv4_hdr(pos, data_out, iph);
	} else if (is_IPv6) {
		pos += IPv6_HDR_LEN;
		write_IPv6_hdr(pos, data_out, ipv6h);
	}

	write_remaining_packet(pos, data_tmp3, data_out, size);
	return 0;
}


// https://github.com/torvalds/linux/blob/master/samples/bpf/xdp2_kern.c
inline int xdp2_OPTIMIZED(const uint8_t data_in[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], unsigned int size_in, unsigned int *size_out, long rxcnt[256]) {
	// #pragma HLS pipeline // It is already in the top function!

	#pragma HLS INTERFACE mode=ap_fifo port=data_in
	#pragma HLS INTERFACE mode=ap_fifo port=data_out

	#pragma HLS interface mode=ap_vld port=size_in
	#pragma HLS interface mode=ap_ovld port=size_out

	#ifndef __SYNTHESIS__
		uint8_t *data_tmp = (uint8_t *) malloc(MAX_PACKET_LEN);
		uint8_t *data_tmp2 = (uint8_t *) malloc(MAX_PACKET_LEN);
		uint8_t *data_tmp3 = (uint8_t *) malloc(MAX_PACKET_LEN);
	#else
		uint8_t data_tmp[MAX_PACKET_LEN];
		uint8_t data_tmp2[MAX_PACKET_LEN];
		uint8_t data_tmp3[MAX_PACKET_LEN];
	#endif

	#pragma HLS array_partition variable=data_in complete
	#pragma HLS array_partition variable=data_out complete
	#pragma HLS array_partition variable=data_tmp complete
	#pragma HLS array_partition variable=data_tmp2 complete
	#pragma HLS array_partition variable=data_tmp3 complete

	*size_out = size_in;
	copy_array(data_in	, data_tmp, data_tmp2, data_tmp3, size_in);

	int action = XDP_PASS;

	union ethhdr_byte_data eth;
	union vlanhdr_byte_data vlan1;
	union vlanhdr_byte_data vlan2;
	union iphdr_byte_data iph;
	union ipv6hdr_byte_data ipv6h;

	int num_vlan = 0;
	int is_IPv4, is_IPv6;

	int err = parsing_task(data_tmp, data_tmp2, size_in, rxcnt, &eth, &vlan1, &vlan2, &iph, &ipv6h, &num_vlan, &is_IPv4, &is_IPv6, &action);
	if (err < 0) return -1;

	err = writing_task(data_out, data_tmp3, size_in, &eth, &vlan1, &vlan2, &iph, &ipv6h, num_vlan, is_IPv4, is_IPv6);

	#ifndef __SYNTHESIS__
		free(data_tmp);
		free(data_tmp2);
		free(data_tmp3);
	#endif

	return action;
}


inline int xdp2_NON_OPTIMIZED(const uint8_t data[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], unsigned int size, long rxcnt[256]) {
	int action = XDP_PASS;

	union ethhdr_byte_data eth;
	union vlanhdr_byte_data vlan1;
	union vlanhdr_byte_data vlan2;
	union iphdr_byte_data iph;
	union ipv6hdr_byte_data ipv6h;

	int size_pipelining, pos, num_vlan;
	num_vlan = pos = 0;
	size_pipelining = size;

	int nh_type = parse_ethhdr(&pos, data, &eth, size); // nh_type=8
	if (nh_type == -1) return -1;

	if (nh_type == htobe16(ETH_P_8021Q) || nh_type == htobe16(ETH_P_8021AD)) {
		nh_type = parse_vlanhdr(&pos, data, &vlan1, size);
		num_vlan++;
	}

	if (nh_type == htobe16(ETH_P_8021Q) || nh_type == htobe16(ETH_P_8021AD)) {
		nh_type = parse_vlanhdr(&pos, data, &vlan2, size);
		num_vlan++;
	}

	unsigned int nh_type2;

	if (nh_type == htobe16(ETH_P_IP)) {
		nh_type2 = parse_iphdr(&pos, data, &iph, size);
	} else if (nh_type == htobe16(ETH_P_IPV6)) {
		nh_type2 = parse_ipv6hdr(&pos, data, &ipv6h, size);
	} else {
		nh_type2 = 0;
	}

	rxcnt[15] += 1;

	if (nh_type2 == IPPROTO_UDP) { // NO htobe16 as it is only 8 bits long
		swap_src_dst_mac_1(&eth);
		action = XDP_TX;
	}

	pos = ETH_HLEN;
	if (write_Eth_hdr(pos, data_out, &eth) != 0) return -1;

	if (num_vlan > 0) {
		pos += VLAN_HDR_LEN;
		if (write_VLAN_hdr(pos, data_out, &vlan1) != 0) return -1;
	}

	if (num_vlan > 1) {
		pos += VLAN_HDR_LEN;
		if (write_VLAN_hdr(pos, data_out, &vlan2) != 0) return -1;
	}

	if (nh_type == htobe16(ETH_P_IP)) {
		pos += iph.iph.ihl*4;
		if (write_IPv4_hdr(pos, data_out, &iph) != 0) return -1;

	} else if (nh_type == htobe16(ETH_P_IPV6)) {
		pos += IPv6_HDR_LEN;
		if (write_IPv6_hdr(pos, data_out, &ipv6h) != 0) return -1;
	}
	if (write_remaining_packet(pos, data, data_out, size) != 0) return -1;

	return action;
}

int xdp2(const uint8_t data_in[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], unsigned int size_in, unsigned int *size_out, long rxcnt[256]) {
	#if OPTIMIZATION
	#pragma HLS pipeline // Otherwise there are one/two cycles of overhead in the optimized version
	#endif
	if (!OPTIMIZATION) return xdp2_NON_OPTIMIZED(data_in, data_out, size_in, rxcnt);
	return xdp2_OPTIMIZED(data_in, data_out, size_in, size_out, rxcnt);
}
