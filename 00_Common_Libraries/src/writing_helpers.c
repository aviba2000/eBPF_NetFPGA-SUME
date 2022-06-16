#include "writing_helpers.h"

// Precondition: pos has to be just at the end of the Ethernet header
int write_Eth_hdr_non_optimized(int pos, uint8_t data[MAX_PACKET_LEN], const union ethhdr_byte_data *ethhdr) {
	int i;
	pos -= ETH_HLEN;
	if (pos < 0) return -1;
	for (i = 0; i < ETH_HLEN; i++)
		data[pos++] = ethhdr->bytes[i];
	return 0;
}

// Precondition: pos has to be just at the end of the Ethernet header
int write_Eth_hdr_optimized(int pos, uint8_t data[MAX_PACKET_LEN], const union ethhdr_byte_data *ethhdr) {
	int i;
	pos -= ETH_HLEN;
	// if (pos < 0) return -1;
	for (i = 0; i < ETH_HLEN; i++) {
		PRINT_DEBUG(", %d", i);
		data[pos++] = ethhdr->bytes[i];
	}
	PRINT_DEBUG("\n");
	return 0;
}

// Precondition: pos has to be just at the end of the VLAN header
int write_VLAN_hdr_non_optimized(int pos, uint8_t data[MAX_PACKET_LEN], const union vlanhdr_byte_data *vlanhdr) {
	int i;
	pos -= VLAN_HDR_LEN;
	if (pos < 0) return -1;
	for (i = 0; i < VLAN_HDR_LEN; i++)
		data[pos++] = vlanhdr->bytes[i];
	return 0;
}

// Precondition: pos has to be just at the end of the VLAN header
int write_VLAN_hdr_optimized(int pos, uint8_t data[MAX_PACKET_LEN], const union vlanhdr_byte_data *vlanhdr) {
	int i;
	pos -= VLAN_HDR_LEN;
	// if (pos < 0) return -1;
	if (pos != -5) {
		for (i = 0; i < VLAN_HDR_LEN; i++) {
				PRINT_DEBUG(", %d", pos);
				data[pos++] = vlanhdr->bytes[i];
		}
	}
	return 0;
}

// Precondition: pos has to be just at the end of the IP header
int write_IPv4_hdr_non_optimized(int pos, uint8_t data[MAX_PACKET_LEN], const union iphdr_byte_data *iphdr) {
	int hdrsize = iphdr->iph.ihl * 4;
	pos -= hdrsize;
	if (pos < 0) return -1;
	int i;
	for (i = 0; i < hdrsize; i++)
		data[pos++] = iphdr->bytes[i];
	return 0;
}

// Precondition: pos has to be just at the end of the IP header
int write_IPv4_hdr_optimized(int pos, uint8_t data[MAX_PACKET_LEN], const union iphdr_byte_data *iphdr) {
	int i;
	int hdrsize = IPv4_BASE_LEN;
	pos -= hdrsize;
	for (i = 0; i < IPv4_BASE_LEN; i++) {
		data[pos++] = iphdr->bytes[i];
	}
	return 0;
}

// Precondition: pos has to be just at the end of the IPv6 header
int write_IPv6_hdr_non_optimized(int pos, uint8_t data[MAX_PACKET_LEN], const union ipv6hdr_byte_data *ipv6hdr) {
	int i;
	pos -= IPv6_HDR_LEN;
	if (pos < 0) return -1;
	for (i = 0; i < IPv6_HDR_LEN; i++)
		data[pos++] = ipv6hdr->bytes[i];
	return 0;
}

// Precondition: pos has to be just at the end of the IPv6 header
int write_IPv6_hdr_optimized(int pos, uint8_t data[MAX_PACKET_LEN], const union ipv6hdr_byte_data *ipv6hdr) {
	int i;
	pos -= IPv6_HDR_LEN;
	for (i = 0; i < IPv6_HDR_LEN; i++) {
		data[pos++] = ipv6hdr->bytes[i];
	}
	return 0;
}

// Precondition: pos has to be just at the end of the ICMP header
int write_ICMP_hdr_optimized(int pos, uint8_t data[MAX_PACKET_LEN], const union icmphdr_byte_data *icmph) {
	int i;
	pos -= ICMP_HDR_LEN;
	for (i = 0; i < ICMP_HDR_LEN; i++) {
		data[pos++] = icmph->bytes[i];
	}
	return 0;
}


// Precondition: pos has to be just at the beginning of the first byte to copy
int write_remaining_packet_non_optimized(int pos, const uint8_t data[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], int size) {
	int i;
	if (pos >= size) return -1;
	for (i = pos; i < size; i++)
		data_out[i] = data[i];
	return 0;
}

int write_remaining_packet_optimized(int pos, const uint8_t data[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], int size) {
	int i;
	for (i = 0; i < MAX_PACKET_LEN; i++) {
		if (i >= pos && i < size) {
			data_out[i] = data[i];
		} else if (i >= pos) data_out[i] = 0;
	}
	return 0;
}

int write_remaining_packet_2_indexes_non_optimized(int pos_data, int pos, const uint8_t data[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], int size) {
	int i;
	int diff = pos_data-pos;
	for (i = pos; i < size; i++) {
		data_out[i] = data[i+diff]; // pos_data++ disables unrolling+array_partitioning and makes this function very slow
	}
	return 0;
}

int write_remaining_packet_2_indexes_optimized(int pos_data, int pos, const uint8_t data[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], int size) {
	int i;
	int diff = pos-pos_data;
	int max_i = size+diff;
	for (i = 0; i < MAX_PACKET_LEN; i++) {
		if (i >= pos_data && i < max_i && diff >= 0) {
			data_out[i+diff] = data[i];
		}
	}
	return 0;
}


int write_Eth_hdr(int pos, uint8_t data[MAX_PACKET_LEN], const union ethhdr_byte_data *ethhdr) {
	if (!OPTIMIZATION) return write_Eth_hdr_non_optimized(pos, data, ethhdr);
	return write_Eth_hdr_optimized(pos, data, ethhdr);
}

int write_VLAN_hdr(int pos, uint8_t data[MAX_PACKET_LEN], const union vlanhdr_byte_data *vlanhdr) {
	if (!OPTIMIZATION) return write_VLAN_hdr_non_optimized(pos, data, vlanhdr);
	return write_VLAN_hdr_optimized(pos, data, vlanhdr);
}

int write_IPv4_hdr(int pos, uint8_t data[MAX_PACKET_LEN], const union iphdr_byte_data *iphdr) {
	if (!OPTIMIZATION) return write_IPv4_hdr_non_optimized(pos, data, iphdr);
	return write_IPv4_hdr_optimized(pos, data, iphdr);
}

int write_IPv6_hdr(int pos, uint8_t data[MAX_PACKET_LEN], const union ipv6hdr_byte_data *ipv6hdr) {
	if (!OPTIMIZATION) return write_IPv6_hdr_non_optimized(pos, data, ipv6hdr);
	return write_IPv6_hdr_optimized(pos, data, ipv6hdr);
}

int write_ICMP_hdr(int pos, uint8_t data[MAX_PACKET_LEN], const union icmphdr_byte_data *icmph) {
	return write_ICMP_hdr_optimized(pos, data, icmph);
}

int write_remaining_packet(int pos, const uint8_t data[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], int size) {
	if (!OPTIMIZATION) return write_remaining_packet_non_optimized(pos, data, data_out, size);
	return write_remaining_packet_optimized(pos, data, data_out, size);
}

int write_remaining_packet_2_indexes(int pos_data, int pos, const uint8_t data[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], int size) {
	if (!OPTIMIZATION) return write_remaining_packet_2_indexes_non_optimized(pos_data, pos, data, data_out, size);
	return write_remaining_packet_2_indexes_optimized(pos_data, pos, data, data_out, size);
}


