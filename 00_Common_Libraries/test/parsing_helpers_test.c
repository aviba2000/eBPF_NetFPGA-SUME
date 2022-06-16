#include "../src/parsing_helpers.h"
#include "parsing_helpers_test.h"

enum Testing_parsing {
	all,
	parse_ethhdr_func,
	parse_vlanhdr_func,
	parse_iphdr_func,
	parse_icmphdr_func,
	parse_ipv6hdr_func
};

enum Testing_parsing test_parsing = all;

int parse_ethhdr_test() {
	// Test 1: Testing the parsing capabilities
	printf("Starting parse_ethhdr Test 1: Testing the parsing capabilities\n");
	int size = 98;
	uint8_t original_in_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,92,244,64,0,64,1,197,156,10,11,2,2,10,11,2,1,8,0,252,241,169,228,0,3,181,188,39,98,130,4,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = original_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	int orig_pos, pos;
	union ethhdr_byte_data eth;
	orig_pos = pos = 0;
	int nh_type = parse_ethhdr(&pos, in_val, &eth, size);
	if (nh_type != htobe16(ETH_P_IP)) return -1;
	if (pos != 14) return -2;
	for (i = orig_pos; i < pos; i++) if (in_val[i] != eth.bytes[i-orig_pos]) return -3;
	for (i = 0; i < size; i++) if (in_val[i] != original_in_val[i]) return -4;
	printf("parse_ethhdr Test 1 completed with success!\n");

	// Test 2: The Eth header does not fit
	printf("Starting parse_ethhdr Test 2: The Eth header does not fit\n");
	pos = 90;
	nh_type = parse_ethhdr(&pos, in_val, &eth, size);
	if (nh_type != -1) return -5;
	printf("parse_ethhdr Test 2 completed with success!\n");
	return 0;
}

int parse_vlanhdr_test() {
	// Test 1: Testing the parsing capabilities
	printf("Starting parse_vlanhdr Test 1: Testing the parsing capabilities\n");
	int size = 122;
	uint8_t original_in_val[122] = {42,201,38,67,187,184,94,19,125,250,104,199,129,0,0,1,134,221,96,12,209,7,0,64,58,64,252,0,222,173,202,254,0,4,0,0,0,0,0,0,0,2,252,0,222,173,202,254,0,4,0,0,0,0,0,0,0,1,128,0,122,252,227,51,0,2,199,52,50,98,228,82,13,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = original_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	int orig_pos, pos;
	union vlanhdr_byte_data vlan;
	orig_pos = pos = 14;
	unsigned short nh_type = parse_vlanhdr(&pos, in_val, &vlan, size);
	if (nh_type != htobe16(ETH_P_IPV6)) return -1;
	if (pos != 18) return -2;
	for (i = 0; i < size; i++) if (in_val[i] != original_in_val[i]) return -3;
	printf("parse_vlanhdr Test 1 completed with success!\n");

	// Test 2: The VLAN header does not fit
	printf("Starting parse_vlanhdr Test 2: The VLAN header does not fit\n");
	pos = 119;
	nh_type = parse_vlanhdr(&pos, in_val, &vlan, size);
	if ((short)nh_type != -1) return -4;
	printf("parse_vlanhdr Test 2 completed with success!\n");
	return 0;
}

int parse_iphdr_test() {
	// Test 1: Testing the parsing capabilities
	printf("Starting parse_iphdr Test 1: Testing the parsing capabilities\n");
	int size = 98;
	uint8_t original_in_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,92,244,64,0,64,1,197,156,10,11,2,2,10,11,2,1,8,0,252,241,169,228,0,3,181,188,39,98,130,4,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i, orig_pos, pos;
	for (i = 0; i < size; i++) in_val[i] = original_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	union iphdr_byte_data iph;
	orig_pos = pos = 14;
	int nh_type = parse_iphdr(&pos, in_val, &iph, size);
	if (nh_type != 1) return -1;
	if (pos != 34) return -2;
	for (i = orig_pos; i < pos; i++) if (in_val[i] != iph.bytes[i-orig_pos]) return -3;
	for (i = 0; i < size; i++) if (in_val[i] != original_in_val[i]) return -4;
	printf("parse_iphdr Test 1 completed with success!\n");

	// Test 2: The IPv4 header does not fit
	printf("Starting parse_iphdr Test 2: The IPv4 header does not fit\n");
	pos = 90;
	nh_type = parse_iphdr(&pos, in_val, &iph, size);
	if (nh_type != -1) return -5;
	printf("parse_iphdr Test 2 completed with success!\n");
	return 0;
}

int parse_ipv6hdr_test() {
	// Test 1: Testing the parsing capabilities
	printf("Starting parse_ipv6hdr Test 1: Testing the parsing capabilities\n");
	int size = 118;
	uint8_t original_in_val[118] = {66, 167, 140, 216, 215, 172, 178, 205, 176, 120, 134, 83, 134, 221, 96, 1, 181, 47, 0, 64, 58, 64, 252, 0, 222, 173, 202, 254, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 252, 0, 222, 173, 202, 254, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 174, 10, 7, 144, 0, 1, 3, 225, 31, 98, 103, 67, 9, 0, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i, orig_pos, pos;
	for (i = 0; i < size; i++) in_val[i] = original_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	union ipv6hdr_byte_data ipv6h;
	orig_pos = pos = 14;
	int nh_type = parse_ipv6hdr(&pos, in_val, &ipv6h, size);
	if (nh_type != 58) return -1;
	if (pos != 54) return -2;
	for (i = orig_pos; i < pos; i++) if (in_val[i] != ipv6h.bytes[i-orig_pos]) return -3;
	for (i = 0; i < size; i++) if (in_val[i] != original_in_val[i]) return -4;
	printf("parse_ipv6hdr Test 1 completed with success!\n");

	// Test 2: The IPv6 header does not fit
	printf("Starting parse_ipv6hdr Test 2: The IPv6 header does not fit\n");
	pos = 90;
	nh_type = parse_ipv6hdr(&pos, in_val, &ipv6h, size);
	if (nh_type != -1) return -5;
	printf("parse_ipv6hdr Test 2 completed with success!\n");
	return 0;
}

int parse_icmphdr_test() {
	// Test 1: Testing the parsing capabilities
	printf("Starting parse_icmphdr Test 1: Testing the parsing capabilities\n");
	int size = 98;
	uint8_t original_in_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,126,154,64,0,64,1,163,246,10,11,2,2,10,11,2,1,8,0,163,221,133,158,0,2,122,81,42,98,50,203,12,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = original_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	int orig_pos, pos;
	union icmphdr_byte_data icmph;
	orig_pos = pos = 90;
	int nh_type = parse_icmphdr(&pos, in_val, &icmph, size);
	printf("nh_type = %d\n", nh_type);
	if (nh_type != 48) return -1;
	if (pos != 98) return -2;
	for (i = orig_pos; i < pos; i++) if (in_val[i] != icmph.bytes[i-orig_pos]) return -3;
	for (i = 0; i < size; i++) if (in_val[i] != original_in_val[i]) return -4;
	printf("parse_icmphdr Test 1 completed with success!\n");

	// Test 2: The Eth header does not fit
	printf("Starting parse_icmphdr Test 2: The Eth header does not fit\n");
	pos = 91;
	nh_type = parse_icmphdr(&pos, in_val, &icmph, size);
	
	if (nh_type != -1) return -5;
	printf("parse_icmphdr Test 2 completed with success!\n");
	return 0;
}

int main_parsing() {
	// All the testing functions return 0 if everything is OK, any another value (< 0) otherwise.
	int ret = 1;
	switch(test_parsing) {
		case all:
			ret = parse_ethhdr_test();
			if (ret < 0) break;
			ret = parse_vlanhdr_test();
			if (ret < 0) break;
			ret += parse_iphdr_test();
			if (ret < 0) break;
			ret += parse_icmphdr_test();
			if (ret < 0) break;
			ret += parse_ipv6hdr_test();
			break;
		case parse_ethhdr_func:
			ret = parse_ethhdr_test();
			break;
		case parse_vlanhdr_func:
			ret = parse_vlanhdr_test();
			break;
		case parse_iphdr_func:
			ret = parse_iphdr_test();
			break;
		case parse_icmphdr_func:
			ret = parse_icmphdr_test();
			break;
		case parse_ipv6hdr_func:
			ret = parse_ipv6hdr_test();
			break;
	}
	printf("Return value: %d\n", ret);
	return ret;
}
