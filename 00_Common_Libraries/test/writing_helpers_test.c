#include "../src/writing_helpers.h"
#include "writing_helpers_test.h"

enum Testing_writing {
	all,
	write_Eth_hdr_func,
	write_VLAN_hdr_func,
	write_IPv4_hdr_func,
	write_IPv6_hdr_func,
	write_ICMP_hdr_func,
	write_remaining_packet_func
};

enum Testing_writing test_writing = all;

int write_Eth_hdr_test() {
	// Test 1: Modifying the source Ethernet address
	printf("Starting write_Eth_hdr Test 1: Modifying the source Ethernet address\n");
	int size = 98;
	uint8_t data_in_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,92,244,64,0,64,1,197,156,10,11,2,2,10,11,2,1,8,0,252,241,169,228,0,3,181,188,39,98,130,4,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = data_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	uint8_t out_val[98] = {174,234,0,2,63,15,246,32,182,253,7,73,8,0,69,0,0,84,92,244,64,0,64,1,197,156,10,11,2,2,10,11,2,1,8,0,252,241,169,228,0,3,181,188,39,98,130,4,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	int pos = ETH_HLEN;
	union ethhdr_byte_data ethhdr;
	uint8_t new_eth_hdr[ETH_HLEN] = {174,234,0,2,63,15,246,32,182,253,7,73,8,0};
	for (i = 0; i < ETH_HLEN; i++) ethhdr.bytes[i] = new_eth_hdr[i];
	int nh_type = write_Eth_hdr(pos, in_val, &ethhdr);
	if (nh_type != 0) return -1;
	for (i = pos-ETH_HLEN; i < pos; i++) if (in_val[i] != out_val[i]) return -2;
	printf("write_Eth_hdr Test 1 completed with success!\n");
	return 0;
}

int write_VLAN_hdr_test() {
	// Test 1: Modifying the VLAN header
	printf("Starting write_VLAN_hdr Test 1: Modifying the VLAN header\n");
	int size = 122;
	uint8_t data_in_val[122] = {42,201,38,67,187,184,94,19,125,250,104,199,129,0,0,1,134,221,96,12,209,7,0,64,58,64,252,0,222,173,202,254,0,4,0,0,0,0,0,0,0,2,252,0,222,173,202,254,0,4,0,0,0,0,0,0,0,1,128,0,122,252,227,51,0,2,199,52,50,98,228,82,13,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = data_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	uint8_t out_val[122] = {42,201,38,67,187,184,94,19,125,250,104,199,129,0,1,2,3,4,96,12,209,7,0,64,58,64,252,0,222,173,202,254,0,4,0,0,0,0,0,0,0,2,252,0,222,173,202,254,0,4,0,0,0,0,0,0,0,1,128,0,122,252,227,51,0,2,199,52,50,98,228,82,13,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	int pos = ETH_HLEN+VLAN_HDR_LEN;
	union vlanhdr_byte_data vlanhdr;
	uint8_t new_vlan_hdr[VLAN_HDR_LEN] = {1,2,3,4};
	for (i = 0; i < ETH_HLEN; i++) vlanhdr.bytes[i] = new_vlan_hdr[i];
	int nh_type = write_VLAN_hdr(pos, in_val, &vlanhdr);
	if (nh_type != 0) return -1;
	for (i = pos-VLAN_HDR_LEN; i < pos; i++) if (in_val[i] != out_val[i]) return -2;
	printf("write_VLAN_hdr Test 1 completed with success!\n");
	return 0;
}

int write_IPv4_hdr_test() {
	// Test 1: Modifying the source IPv4 address and, consequently, the checksum
	printf("Starting write_IPv4_hdr Test 1: Modifying the source IPv4 address and, consequently, the checksum\n");
	int size = 98;
	uint8_t data_in_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,92,244,64,0,64,1,197,156,10,11,2,2,10,11,2,1,8,0,252,241,169,228,0,3,181,188,39,98,130,4,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = data_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	uint8_t out_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,92,244,64,0,64,1,207,188,128,138,129,98,10,11,2,1,8,0,252,241,169,228,0,3,181,188,39,98,130,4,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	int pos = 34;
	union iphdr_byte_data iph;
	uint8_t new_ip_hdr[20] = {69,0,0,84,92,244,64,0,64,1,207,188,128,138,129,98,10,11,2,1};
	for (i = 0; i < 20; i++) iph.bytes[i] = new_ip_hdr[i];
	int nh_type = write_IPv4_hdr(pos, in_val, &iph);
	if (nh_type != 0) return -1;
	for (i = pos-20; i < pos; i++) if (in_val[i] != out_val[i]) return -2;
	printf("write_IPv4_hdr Test 1 completed with success!\n");
	return 0;
}

int write_IPv6_hdr_test() {
	// Test 1: Modifying the source IPv6 address
	printf("Starting write_IPv6_hdr Test 1: Modifying the source IPv6 address\n");
	int size = 118;
	uint8_t data_in_val[118] = {66, 167, 140, 216, 215, 172, 178, 205, 176, 120, 134, 83, 134, 221, 96, 1, 181, 47, 0, 64, 58, 64, 252, 0, 222, 173, 202, 254, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 252, 0, 222, 173, 202, 254, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 174, 10, 7, 144, 0, 1, 3, 225, 31, 98, 103, 67, 9, 0, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = data_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	uint8_t out_val[118] = {66, 167, 140, 216, 215, 172, 178, 205, 176, 120, 134, 83, 134, 221, 96, 1, 181, 47, 0, 64, 58, 64, 2, 0, 45, 3, 3, 254, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 252, 0, 222, 173, 202, 254, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 174, 10, 7, 144, 0, 1, 3, 225, 31, 98, 103, 67, 9, 0, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55};
	int pos = 54;
	union ipv6hdr_byte_data ipv6h;
	uint8_t new_ipv6_hdr[40] = {96, 1, 181, 47, 0, 64, 58, 64, 2, 0, 45, 3, 3, 254, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 252, 0, 222, 173, 202, 254, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1};// , 128, 0, 174, 10, 7, 144, 0, 1, 3, 225, 31, 98, 103, 67, 9, 0, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55};
	for (i = 0; i < 40; i++) ipv6h.bytes[i] = new_ipv6_hdr[i];
	int nh_type = write_IPv6_hdr(pos, in_val, &ipv6h);
	if (nh_type != 0) return -1;
	for (i = pos-40; i < pos; i++) if (in_val[i] != out_val[i]) return -2;
	printf("write_IPv6_hdr Test 1 completed with success!\n");
	return 0;
}

int write_ICMP_hdr_test() {
	// Test 1: Modifying the ICMP header
	printf("Starting write_ICMP_hdr Test 1: Modifying the ICMP header\n");
	int size = 98;
	uint8_t data_in_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,126,154,64,0,64,1,163,246,10,11,2,2,10,11,2,1,8,0,163,221,133,158,0,2,122,81,42,98,50,203,12,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,3,50,3,52,2,54,34};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = data_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	uint8_t out_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,126,154,64,0,64,1,163,246,10,11,2,2,10,11,2,1,8,0,163,221,133,158,0,2,122,81,42,98,50,203,12,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	int pos = 98;
	union icmphdr_byte_data icmph;
	uint8_t new_icmp_hdr[ICMP_HDR_LEN] = {48,49,50,51,52,53,54,55};
	for (i = 0; i < ICMP_HDR_LEN; i++) icmph.bytes[i] = new_icmp_hdr[i];
	int nh_type = write_ICMP_hdr(pos, in_val, &icmph);
	if (nh_type != 0) return -1;
	for (i = pos-ICMP_HDR_LEN; i < pos; i++) if (in_val[i] != out_val[i]) return -2;
	printf("write_ICMP_hdr Test 1 completed with success!\n");
	return 0;
}

int write_remaining_packet_test() {
	// Test 1: Modifying a random value
	printf("Starting write_remaining_packet Test 1: Modifying a random value\n");
	int size = 98;
	uint8_t data_in_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,92,244,64,0,64,1,197,156,10,11,2,2,10,11,2,1,8,0,252,241,169,228,0,3,181,188,39,98,130,4,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,1,2,3};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = data_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	uint8_t data_out_val[98] = {174,234,0,2,63,15,246,32,182,253,7,73,8,0,69,0,0,84,92,244,64,0,64,1,197,156,10,11,2,2,10,11,2,1,8,0,252,241,169,228,0,3,181,188,39,98,130,4,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	uint8_t out_val[MAX_PACKET_LEN];
	for (i = 0; i < size; i++) out_val[i] = data_out_val[i];
	for (; i < MAX_PACKET_LEN; i++) out_val[i] = 0;
	uint8_t out_val_sol[98] = {174,234,0,2,63,15,246,32,182,253,7,73,8,0,69,0,0,84,92,244,64,0,64,1,197,156,10,11,2,2,10,11,2,1,8,0,252,241,169,228,0,3,181,188,39,98,130,4,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,1,2,3};
	int pos = 34;
	int nh_type = write_remaining_packet(pos, in_val, out_val, size);
	if (nh_type != 0) return -1;
	// for (i = 0; i < MAX_PACKET_LEN; i++) printf("i = %d, out_val = %d\n", i , (int)out_val[i]);
	for (i = 34; i < size; i++) if (out_val[i] != out_val_sol[i]) {
		printf("At i = %d, out_val = %d, sol = %d\n", i, out_val[i], out_val_sol[i]);
		return -2;
	}
	printf("write_remaining_packet Test 1 completed with success!\n");
	return 0;
}


int main_writing() {
	// All the testing functions return 0 if everything is OK, any another value (< 0) otherwise.
	int ret = 1;
	switch(test_writing) {
		case all:
			ret = write_Eth_hdr_test();
			if (ret < 0) break;
			ret += write_VLAN_hdr_test();
			if (ret < 0) break;
			ret += write_IPv4_hdr_test();
			if (ret < 0) break;
			ret += write_IPv6_hdr_test();
			if (ret < 0) break;
			ret += write_ICMP_hdr_test();
			if (ret < 0) break;
			ret += write_remaining_packet_test();
			break;
		case write_Eth_hdr_func:
			ret = write_Eth_hdr_test();
			break;
		case write_VLAN_hdr_func:
			ret = write_VLAN_hdr_test();
			break;
		case write_IPv4_hdr_func:
			ret = write_IPv4_hdr_test();
			break;
		case write_IPv6_hdr_func:
			ret = write_IPv6_hdr_test();
			break;
		case write_ICMP_hdr_func:
			ret = write_ICMP_hdr_test();
			break;
		case write_remaining_packet_func:
			ret = write_remaining_packet_test();
			break;
	}
	printf("Return value: %d\n", ret);
	return ret;
}
