#include "parsing_helpers.h"

int parse_ethhdr_non_optimized(int *pos, const uint8_t data[MAX_PACKET_LEN], union ethhdr_byte_data *ethhdr, int size) {
	int i, position = *pos;
	if (position + ETH_HLEN > size) return -1;
	for (i = 0; i < ETH_HLEN; i++)
		ethhdr->bytes[i] = data[position++];
	*pos = position;
	return ethhdr->eth.h_proto;
}

int parse_ethhdr_optimized(int *pos, const uint8_t data[MAX_PACKET_LEN], union ethhdr_byte_data *ethhdr, int size) {
	int i, position = *pos;
	if (position + ETH_HLEN > size) return -1;
	for (i = 0; i < ETH_HLEN; i++) {
		ethhdr->bytes[i] = data[position++];
	}
	*pos = position;
	return ethhdr->eth.h_proto;
}

unsigned short parse_vlanhdr_non_optimized(int *pos, const uint8_t data[MAX_PACKET_LEN], union vlanhdr_byte_data *vlanhdr, int size) {
	int i, position = *pos;
	if (position + VLAN_HDR_LEN > size) return -1;
	if (size != -1) {
		for (i = 0; i < VLAN_HDR_LEN; i++)
			vlanhdr->bytes[i] = data[position+i];
		*pos = position+i;
	}
	if (size == -1) return -2;
	return vlanhdr->vlan.h_vlan_encapsulated_proto;
}

unsigned short parse_vlanhdr_optimized(int *pos, const uint8_t data[MAX_PACKET_LEN], union vlanhdr_byte_data *vlanhdr, int size) {
	int i, position = *pos;
	if (position + VLAN_HDR_LEN > size) return -1;
	if (size != -1) {
		for (i = 0; i < VLAN_HDR_LEN; i++) {
				vlanhdr->bytes[i] = data[position++]; // position + i makes urem (unsigned reminder) make the design slow
		}
	}
	*pos = position;
	if (size == -1) return -2;
	return vlanhdr->vlan.h_vlan_encapsulated_proto;

}

int parse_iphdr_non_optimized(int *pos, const uint8_t data[MAX_PACKET_LEN], union iphdr_byte_data *iphdr, int size) {
	int i, position = *pos;
	if (position + IPv4_BASE_LEN > size) return -1;
	for (i = 0; i < IPv4_BASE_LEN; i++)
		iphdr->bytes[i] = data[position++];
	int hdrsize = iphdr->iph.ihl * 4;
	if(hdrsize < IPv4_BASE_LEN) return -1;
	if (*pos + hdrsize > size) return -1;
	for (; i < hdrsize; i++)
		iphdr->bytes[i] = data[position++];
	*pos = position;
	return iphdr->iph.protocol;
}

int parse_iphdr_optimized(int *pos, const uint8_t data[MAX_PACKET_LEN], union iphdr_byte_data *iphdr, int size) {
	int i, position = *pos;
	if (position + IPv4_BASE_LEN > size) return -1;
	for (i = 0; i < IPv4_BASE_LEN; i++) {
		iphdr->bytes[i] = data[position++];
	}
	*pos = position;
	return iphdr->iph.protocol;
}

int parse_ipv6hdr_non_optimized(int *pos, const uint8_t data[MAX_PACKET_LEN], union ipv6hdr_byte_data *ipv6hdr, int size) {
	int i, position = *pos;
	if (position + IPv6_HDR_LEN > size) return -1;
	for (i = 0; i < IPv6_HDR_LEN; i++)
		ipv6hdr->bytes[i] = data[position++];
	*pos = position;
	return (int)ipv6hdr->ipv6h.nexthdr;
}

int parse_ipv6hdr_optimized(int *pos, const uint8_t data[MAX_PACKET_LEN], union ipv6hdr_byte_data *ipv6hdr, int size) {
	int i, position = *pos;
	if (position + IPv6_HDR_LEN > size) return -1;
	for (i = 0; i < IPv6_HDR_LEN; i++) {
		PRINT_DEBUG(", %d", position);
		ipv6hdr->bytes[i] = data[position++];
	}
	*pos = position;
	return (int)ipv6hdr->ipv6h.nexthdr;
}

int parse_icmphdr_optimized(int *pos, const uint8_t data[MAX_PACKET_LEN], union icmphdr_byte_data *icmph, int size) {
	int i, position = *pos;
	if (position + ICMP_HDR_LEN > size) return -1;
	for (i = 0; i < ICMP_HDR_LEN; i++) {
		PRINT_DEBUG(", %d", position);
		icmph->bytes[i] = data[position++];
	}
	*pos = position;
	return icmph->icmph.type;
}

int parse_ethhdr(int *pos, const uint8_t data[MAX_PACKET_LEN], union ethhdr_byte_data *ethhdr, int size) {
	if (!OPTIMIZATION) return parse_ethhdr_non_optimized(pos, data, ethhdr, size);
	return parse_ethhdr_optimized(pos, data, ethhdr, size);
}

unsigned short parse_vlanhdr(int *pos, const uint8_t data[MAX_PACKET_LEN], union vlanhdr_byte_data *vlanhdr, int size) {
	if (!OPTIMIZATION) return parse_vlanhdr_non_optimized(pos, data, vlanhdr, size);
	return parse_vlanhdr_optimized(pos, data, vlanhdr, size);
}

int parse_iphdr(int *pos, const uint8_t data[MAX_PACKET_LEN], union iphdr_byte_data *iphdr, int size) {
	if (!OPTIMIZATION) return parse_iphdr_non_optimized(pos, data, iphdr, size);
	return parse_iphdr_optimized(pos, data, iphdr, size);
}

int parse_ipv6hdr(int *pos, const uint8_t data[MAX_PACKET_LEN], union ipv6hdr_byte_data *ipv6hdr, int size) {
	if (!OPTIMIZATION) return parse_ipv6hdr_non_optimized(pos, data, ipv6hdr, size);
	return parse_ipv6hdr_optimized(pos, data, ipv6hdr, size);
}

int parse_icmphdr(int *pos, const uint8_t data[MAX_PACKET_LEN], union icmphdr_byte_data *icmph, int size) {
	return parse_icmphdr_optimized(pos, data, icmph, size);
}
