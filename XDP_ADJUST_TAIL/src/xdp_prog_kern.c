#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/ipv6.h>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "xdp_prog_kern.h"

void swap_src_dst_mac(union ethhdr_byte_data *eth) {
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

int send_icmp4_too_big(union ethhdr_byte_data *eth, union iphdr_byte_data *iph, union icmphdr_byte_data *icmph, unsigned int *size, const uint8_t data_tmp3[MAX_PACKET_LEN]) {
	int headroom = IPv4_BASE_LEN + ICMP_HDR_LEN;

	*size += headroom;

	if (ICMP_TOOBIG_SIZE + headroom > *size) return XDP_DROP;
	
	__u32 csum = 0;
	__u64 off = 0;

	swap_src_dst_mac(eth);
	
	icmph->icmph.type = ICMP_DEST_UNREACH;
	icmph->icmph.code = ICMP_FRAG_NEEDED;
	icmph->icmph.un.frag.mtu = htobe16(max_pcktsz - ETH_HLEN);
	icmph->icmph.checksum = 0;
	
	icmph->icmph.un.reserved[0] = eth->eth.h_dest[4];
	icmph->icmph.un.reserved[1] = eth->eth.h_dest[5];
	
	int i, acc = 0;
	int loop_size = ICMP_HDR_LEN>>1;
	for (i = 0; i < loop_size; i++) {
		acc += (unsigned int) icmph->bytes16[i];
	}

	csum = Checksum_8bits_optimized((uint8_t *) (data_tmp3+ETH_HLEN), ICMP_TOOBIG_PAYLOAD_SIZE-ICMP_HDR_LEN, (unsigned int) acc);

	icmph->icmph.checksum = csum;

	iph->iph.ttl = DEFAULT_TTL;
	__be32 aux = iph->iph.daddr;
	iph->iph.daddr = iph->iph.saddr;
	iph->iph.saddr = aux;
	iph->iph.version = 4;
	iph->iph.ihl = 5;
	iph->iph.protocol = IPPROTO_ICMP;
	iph->iph.tos = 0;
	iph->iph.tot_len = htobe16(ICMP_TOOBIG_SIZE + headroom - ETH_HLEN);

	iph->iph.id = 0; // It was not on the original program -> by default it is not set to zero in Vitis!
	iph->iph.frag_off = 0; // It was not on the original program -> by default it is not set to zero in Vitis!
	
	iph->iph.check = 0;
	csum = 0;
	csum = IPV4Checksum(&(iph->bytes16[0]), IPv4_BASE_LEN, 0);

	iph->iph.check = csum;
	
	return XDP_TX;
}

int handle_ipv4(union ethhdr_byte_data *eth, union iphdr_byte_data *iph, union icmphdr_byte_data *icmph, unsigned int *size, const uint8_t data_tmp3[MAX_PACKET_LEN]) {
	if (*size > max(max_pcktsz, ICMP_TOOBIG_SIZE)) {
		*size = ICMP_TOOBIG_SIZE;
		return send_icmp4_too_big(eth, iph, icmph, size, data_tmp3);
	}
	return XDP_PASS;
}

void copy_array(uint8_t data[MAX_PACKET_LEN], uint8_t data_tmp[MAX_PACKET_LEN], uint8_t data_tmp2[MAX_PACKET_LEN], unsigned int size){
	int i;
	uint8_t aux;
	for (i = 0; i < MAX_PACKET_LEN; i++) {
		if (i < size) {
			PRINT_DEBUG(", %d", i);
			aux = data[i];
			data_tmp[i] = aux;
			data_tmp2[i] = aux;
		}
	}
}

int parsing_task(const uint8_t data_tmp[MAX_PACKET_LEN], unsigned int size, union ethhdr_byte_data *eth, union iphdr_byte_data *iph) {
	int pos = 0;

	int nh_type = parse_ethhdr(&pos, data_tmp, eth, size);
	if (nh_type == -1) return -1;

	if (nh_type == htobe16(ETH_P_IP)) {
		nh_type = parse_iphdr(&pos, data_tmp, iph, size);
		if (nh_type == -1) return -1;
	}

	return 0;
}

int writing_task(uint8_t data_out[MAX_PACKET_LEN], const uint8_t data_tmp2[MAX_PACKET_LEN], unsigned int *size, union ethhdr_byte_data *eth, union iphdr_byte_data *iph, int *action) {
	int pos = ETH_HLEN;
	int pos_data = 0;

	if (eth->eth.h_proto == htobe16(ETH_P_IP)) {
		union icmphdr_byte_data icmph;
		*action = handle_ipv4(eth, iph, &icmph, size, data_tmp2);
		write_Eth_hdr(pos, data_out, eth);
		pos += IPv4_BASE_LEN;
		write_IPv4_hdr(pos, data_out, iph);
		pos += ICMP_HDR_LEN;
		write_ICMP_hdr(pos, data_out, &icmph);
		pos_data = ETH_HLEN; // Headroom: iphdr and icmphdr
		write_remaining_packet_2_indexes(ETH_HLEN, ETH_HLEN+ICMP_HDR_LEN+IPv4_BASE_LEN, data_tmp2, data_out, *size);

	} else {
		write_remaining_packet(0, (uint8_t *) (data_tmp2+ETH_HLEN), data_out, *size);
	}
	return 0;
}

// Source: https://github.com/torvalds/linux/blob/master/samples/bpf/xdp_adjust_tail_kern.c
inline int xdp_prog1_OPTIMIZED(uint8_t data[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], unsigned int in_size, unsigned int *out_size, long rxcnt[256]) {
	// #pragma HLS pipeline // It is already in the top function!

	#pragma HLS INTERFACE mode=ap_fifo port=data
	#pragma HLS INTERFACE mode=ap_fifo port=data_out

	#ifndef __SYNTHESIS__
		uint8_t *data_tmp = (uint8_t *) malloc(MAX_PACKET_LEN);
		uint8_t *data_tmp2 = (uint8_t *) malloc(MAX_PACKET_LEN);
	#else
		uint8_t data_tmp[MAX_PACKET_LEN];
		uint8_t data_tmp2[MAX_PACKET_LEN];
	#endif

	#pragma HLS array_partition variable=data complete
	#pragma HLS array_partition variable=data_out complete
	#pragma HLS array_partition variable=data_tmp complete

	#pragma HLS array_partition variable=data_tmp2 complete

	unsigned int size_cpy = in_size;

	copy_array(data, data_tmp, data_tmp2, in_size);


	int action = XDP_PASS;

	union ethhdr_byte_data eth;
	union iphdr_byte_data iph;

	int err = parsing_task(data_tmp, in_size, &eth, &iph);
	if (err < 0) return -1;

	err = writing_task(data_out, data_tmp2, &size_cpy, &eth, &iph, &action);

	*out_size = size_cpy;

	__u64 icmp_count = rxcnt[0];
	if (icmp_count) icmp_count++;
	rxcnt[0] = icmp_count;

	#ifndef __SYNTHESIS__
		free(data_tmp);
		free(data_tmp2);
	#endif

	return action;
}

// Source: https://github.com/torvalds/linux/blob/master/samples/bpf/xdp_adjust_tail_kern.c
inline int xdp_prog1_NON_OPTIMIZED(uint8_t data[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], unsigned int in_size, unsigned int *out_size, long rxcnt[256]) {
	int action = XDP_PASS;

	union ethhdr_byte_data eth;
	union iphdr_byte_data iph;
	int num_vlan = 0;
	int is_IPv4, is_IPv6;

	int err = parsing_task(data, in_size, &eth, &iph);
	if (err < 0) return -1;

	*out_size = in_size;

	err = writing_task(data_out, data, out_size, &eth, &iph, &action);

	__u64 icmp_count = rxcnt[0];
	if (icmp_count) icmp_count++;
	rxcnt[0] = icmp_count;

	return action;
}

int xdp_prog1(uint8_t data[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], unsigned int in_size, unsigned int *out_size, long rxcnt[256]) {
	#if OPTIMIZATION
	#pragma HLS pipeline // Otherwise there are one/two cycles of overhead in the optimized version
	#endif
	if (!OPTIMIZATION) return xdp_prog1_NON_OPTIMIZED(data, data_out, in_size, out_size, rxcnt);
	else return xdp_prog1_OPTIMIZED(data, data_out, in_size, out_size, rxcnt);
}

