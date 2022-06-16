#include "../src/fast_helpers.h"
#include "fast_helpers_test.h"

enum Testing_fast_function {
	all,
	get_16,
	get_32,
	get_64,
	write_16,
	write_32,
	write_64,
	parse_eth_fast,
	parse_vlan_fast,
	parse_ipv4_fast,
	parse_ipv6_fast
};

enum Testing_fast_function test_fast_function = all;

int get_value16_test() {
	// Test 1: Testing getting a 16 bit value with offset = 0
	printf("Starting get_value16 Test 1: Testing getting a 16 bit value with offset = 0\n");
	uint8_t data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	uint8_t *ppos = data;
	unsigned short res = get_value16(ppos, 0);
	if (res != 513) { // 513 = (2 << 8)+1
		printf("get_value16 Test 1 FAILED! The value read has been %d instead of 513!\n", res);
		return -1;
	}
	printf("get_value16 Test 1 completed with success!\n");
	printf("Starting get_value16 Test 2: Testing getting a 16 bit value with offset = 4\n");
	res = get_value16(ppos, 4);
	if (res != 1541) {
		printf("get_value16 Test 2 FAILED! The value read has been %d instead of 1541!\n", res);
		return -2;
	}
	printf("get_value16 Test 2 completed with success!\n");
	return 0;
}

int get_value32_test() {
	// Test 1: Testing getting a 32 bit value with offset = 0
	printf("Starting get_value32 Test 1: Testing getting a 32 bit value with offset = 0\n");
	uint8_t data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	uint8_t *ppos = data;
	unsigned int res = get_value32(ppos, 0);
	if (res != 67305985) { // 67305985 = (4 << 24) + (3 << 16) + (2 << 8) + 1
		printf("get_value32 Test 1 FAILED! The value read has been %d instead of 67305985!\n", res);
		return -1;
	}
	printf("get_value32 Test 1 completed with success!\n");
	printf("Starting get_value32 Test 2: Testing getting a 32 bit value with offset = 4\n");
	res = get_value32(ppos, 4);
	if (res != 134678021) {
		printf("get_value32 Test 2 FAILED! The value read has been %d instead of 134678021!\n", res);
		return -2;
	}
	printf("get_value32 Test 2 completed with success!\n");
	return 0;
}

int get_value64_test() {
	// Test 1: Testing getting a 64 bit value with offset = 0
	printf("Starting get_value64 Test 1: Testing getting a 64 bit value with offset = 0\n");
	uint8_t data[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
	uint8_t *ppos = data;
	unsigned long res = get_value64(ppos, 0);
	if (res != 578437695752307201) {
		printf("get_value64 Test 1 FAILED! The value read has been %lu instead of 578437695752307201!\n", res);
		return -1;
	}
	printf("get_value64 Test 1 completed with success!\n");
	printf("Starting get_value64 Test 2: Testing getting a 64 bit value with offset = 4\n");
	res = get_value64(ppos, 4);
	if (res != 867798387104613893) {
		printf("get_value64 Test 2 FAILED! The value read has been %lu instead of 867798387104613893!\n", res);
		return -2;
	}
	printf("get_value64 Test 2 completed with success!\n");
	return 0;
}

int write_value16_test() {
	// Test 1: Testing writing a 16 bit value with offset = 0
	printf("Starting write_value16 Test 1: Testing writing a 16 bit value with offset = 0\n");
	uint8_t data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	uint8_t data_copy[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	int size = sizeof(data);
	uint8_t *ppos = data;
	unsigned short val = 770;
	data_copy[0] = 2; // 770 = (3 << 8) + 2
	data_copy[1] = 3;
	write_value16(ppos, 0, val);
	int i;
	for (i = 0; i < size; i++) {
		if (data[i] != data_copy[i]) {
			printf("write_value16 Test 1 FAILED! The value written has been %d instead of %d, at i = %d!\n", data[i], data_copy[i], i);
			return -1;
		}
	}
	printf("write_value16 Test 1 completed with success!\n");
	printf("Starting write_value16 Test 2: Testing writing a 16 bit value with offset = 4\n");
	data_copy[4] = 2;
	data_copy[5] = 3;
	write_value16(ppos, 4, val);
	for (i = 0; i < size; i++) {
		if (data[i] != data_copy[i]) {
			printf("write_value16 Test 1 FAILED! The value written has been %d instead of %d, at i = %d!\n", data[i], data_copy[i], i);
			return -2;
		}
	}
	printf("write_value16 Test 2 completed with success!\n");
	return 0;
}

int write_value32_test() {
	// Test 1: Testing writing a 32 bit value with offset = 0
	printf("Starting write_value32 Test 1: Testing writing a 32 bit value with offset = 0\n");
	uint8_t data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	uint8_t data_copy[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	int size = sizeof(data);
	uint8_t *ppos = data;
	unsigned int val = 84148994;
	data_copy[0] = 2; // 84148994 = 5*2^24+4*2^16+3*2^8+2
	data_copy[1] = 3;
	data_copy[2] = 4;
	data_copy[3] = 5;
	write_value32(ppos, 0, val);
	int i;
	for (i = 0; i < size; i++) {
		if (data[i] != data_copy[i]) {
			printf("write_value32 Test 1 FAILED! The value written has been %d instead of %d, at i = %d!\n", data[i], data_copy[i], i);
			return -1;
		}
	}
	printf("write_value32 Test 1 completed with success!\n");
	printf("Starting write_value32 Test 2: Testing writing a 32 bit value with offset = 4\n");
	data_copy[4] = 2;
	data_copy[5] = 3;
	data_copy[6] = 4;
	data_copy[7] = 5;
	write_value32(ppos, 4, val);
	for (i = 0; i < size; i++) {
		if (data[i] != data_copy[i]) {
			printf("write_value32 Test 1 FAILED! The value written has been %d instead of %d, at i = %d!\n", data[i], data_copy[i], i);
			return -2;
		}
	}
	printf("write_value32 Test 2 completed with success!\n");
	return 0;
}

int write_value64_test() {
	// Test 1: Testing writing a 64 bit value with offset = 0
	printf("Starting write_value64 Test 1: Testing writing a 64 bit value with offset = 0\n");
	uint8_t data[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
	uint8_t data_copy[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
	int size = sizeof(data);
	uint8_t *ppos = data;
	unsigned long val = 650777868590383874;
	data_copy[0] = 2; // 650777868590383874 = 9*2^56+8*2^48+7*2^40+6*2^32+5*2^24+4*2^16+3*2^8+2
	data_copy[1] = 3;
	data_copy[2] = 4;
	data_copy[3] = 5;
	data_copy[4] = 6;
	data_copy[5] = 7;
	data_copy[6] = 8;
	data_copy[7] = 9;
	write_value64(ppos, 0, val);
	int i;
	for (i = 0; i < size; i++) {
		if (data[i] != data_copy[i]) {
			printf("write_value32 Test 1 FAILED! The value written has been %d instead of %d, at i = %d!\n", data[i], data_copy[i], i);
			return -1;
		}
	}
	printf("write_value64 Test 1 completed with success!\n");
	printf("Starting write_value64 Test 2: Testing writing a 64 bit value with offset = 4\n");
	data_copy[4] = 2;
	data_copy[5] = 3;
	data_copy[6] = 4;
	data_copy[7] = 5;
	data_copy[8] = 6;
	data_copy[9] = 7;
	data_copy[10] = 8;
	data_copy[11] = 9;
	write_value64(ppos, 4, val);
	for (i = 0; i < size; i++) {
		if (data[i] != data_copy[i]) {
			printf("write_value64 Test 1 FAILED! The value written has been %d instead of %d, at i = %d!\n", data[i], data_copy[i], i);
			return -2;
		}
	}
	printf("write_value64 Test 2 completed with success!\n");
	return 0;
}

int parse_eth_fast_test() {
	// Test 1: Testing the parsing capabilities
	printf("Starting parse_ethhdr_fast Test 1: Testing the parsing capabilities\n");
	int size = 98;
	uint8_t original_in_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,92,244,64,0,64,1,197,156,10,11,2,2,10,11,2,1,8,0,252,241,169,228,0,3,181,188,39,98,130,4,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = original_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;

	uint8_t *ppos = in_val;
	short nh_type = parse_Eth_fast(&ppos, &(in_val[size-1]));
	if (nh_type != htobe16(ETH_P_IP)) return -1;
	if (ppos != &in_val[14]) return -2;
	for (i = 0; i < size; i++) if (in_val[i] != original_in_val[i]) return -3;
	printf("parse_ethhdr_fast Test 1 completed with success!\n");

	// Test 2: The Eth header does not fit
	printf("Starting parse_ethhdr_fast Test 2: The Eth header does not fit\n");
	ppos = &(in_val[90]);
	nh_type = parse_Eth_fast(&ppos, &(in_val[size-1]));
	if (nh_type != -1) return -4;
	printf("parse_ethhdr_fast Test 2 completed with success!\n");
	return 0;
}

int parse_vlan_fast_test() {
	// Test 1: Testing the parsing capabilities
	printf("Starting parse_VLAN_fast Test 1: Testing the parsing capabilities\n");
	int size = 122;
	uint8_t original_in_val[122] = {42,201,38,67,187,184,94,19,125,250,104,199,129,0,0,1,134,221,96,12,209,7,0,64,58,64,252,0,222,173,202,254,0,4,0,0,0,0,0,0,0,2,252,0,222,173,202,254,0,4,0,0,0,0,0,0,0,1,128,0,122,252,227,51,0,2,199,52,50,98,228,82,13,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = original_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	uint8_t *ppos = &(in_val[14]);
	unsigned short nh_type = parse_VLAN_fast(&ppos, &(in_val[size-1]));
	if (nh_type != htobe16(ETH_P_IPV6)) return -1;
	if (ppos != &in_val[18]) return -2;
	for (i = 0; i < size; i++) if (in_val[i] != original_in_val[i]) return -3;
	printf("parse_VLAN_fast Test 1 completed with success!\n");

	// Test 2: The VLAN header does not fit
	printf("Starting parse_VLAN_fast Test 2: The VLAN header does not fit\n");
	ppos = &(in_val[118]);
	nh_type = parse_VLAN_fast(&ppos, &(in_val[size-1]));
	if ((short)nh_type != -1) return -4;
	printf("parse_VLAN_fast Test 2 completed with success!\n");
	return 0;
}

int parse_ipv4_fast_test() {
	// Test 1: Testing the parsing capabilities
	printf("Starting parse_IPv4_fast Test 1: Testing the parsing capabilities\n");
	int size = 98;
	uint8_t original_in_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,92,244,64,0,64,1,197,156,10,11,2,2,10,11,2,1,8,0,252,241,169,228,0,3,181,188,39,98,130,4,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = original_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	uint8_t *ppos = &(in_val[14]);
	int8_t nh_type = parse_IPv4_fast(&ppos, &(in_val[size-1]));
	if (nh_type != 1) return -1;
	if (ppos != &in_val[34]) return -2;
	for (i = 0; i < size; i++) if (in_val[i] != original_in_val[i]) return -3;
	printf("parse_IPv4_fast Test 1 completed with success!\n");

	// Test 2: The IPv4 header does not fit
	printf("Starting parse_IPv4_fast Test 2: The IPv4 header does not fit\n");
	ppos = &(in_val[90]);
	nh_type = parse_IPv4_fast(&ppos, &(in_val[size-1]));
	if (nh_type != -1) return -5;
	printf("parse_IPv4_fast Test 2 completed with success!\n");
	return 0;
}

int parse_ipv6_fast_test() {
	// Test 1: Testing the parsing capabilities
	printf("Starting parse_IPv6_fast Test 1: Testing the parsing capabilities\n");
	int size = 118;
	uint8_t original_in_val[118] = {66, 167, 140, 216, 215, 172, 178, 205, 176, 120, 134, 83, 134, 221, 96, 1, 181, 47, 0, 64, 58, 64, 252, 0, 222, 173, 202, 254, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 252, 0, 222, 173, 202, 254, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 174, 10, 7, 144, 0, 1, 3, 225, 31, 98, 103, 67, 9, 0, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55};
	uint8_t in_val[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < size; i++) in_val[i] = original_in_val[i];
	for (; i < MAX_PACKET_LEN; i++) in_val[i] = 0;
	uint8_t *ppos = &(in_val[14]);
	int8_t nh_type = parse_IPv6_fast(&ppos, &(in_val[size-1]));
	if (nh_type != 58) return -1;
	if (ppos != &in_val[40+14]) return -2;
	for (i = 0; i < size; i++) if (in_val[i] != original_in_val[i]) return -3;
	printf("parse_IPv6_fast Test 1 completed with success!\n");

	// Test 2: The IPv6 header does not fit
	printf("Starting parse_IPv6_fast Test 2: The IPv6 header does not fit\n");
	ppos = &(in_val[90]);
	nh_type = parse_IPv6_fast(&ppos, &(in_val[size-1]));
	if (nh_type != -1) return -5;
	printf("parse_IPv6_fast Test 2 completed with success!\n");
	return 0;
}

int main_fast_function() {
	// All the testing functions return 0 if everything is OK, any another value (< 0) otherwise.
	int ret = 1;
	switch(test_fast_function) {
		case all:
			ret = get_value16_test();
			if (ret < 0) break;
			ret += get_value32_test();
			if (ret < 0) break;
			ret += get_value64_test();
			if (ret < 0) break;
			ret += write_value16_test();
			if (ret < 0) break;
			ret += write_value32_test();
			if (ret < 0) break;
			ret += write_value64_test();
			if (ret < 0) break;
			ret += parse_eth_fast_test();
			if (ret < 0) break;
			ret += parse_vlan_fast_test();
			if (ret < 0) break;
			ret += parse_ipv4_fast_test();
			if (ret < 0) break;
			ret += parse_ipv6_fast_test();
			break;
		case get_16:
			ret = get_value16_test();
			break;
		case get_32:
			ret = get_value32_test();
			break;
		case get_64:
			ret = get_value64_test();
			break;
		case write_16:
			ret = write_value16_test();
			break;
		case write_32:
			ret = write_value32_test();
			break;
		case write_64:
			ret = write_value64_test();
			break;
		case parse_eth_fast:
			ret = parse_eth_fast_test();
			break;
		case parse_vlan_fast:
			ret = parse_vlan_fast_test();
			break;
		case parse_ipv4_fast:
			ret = parse_ipv4_fast_test();
			break;
		case parse_ipv6_fast:
			ret = parse_ipv6_fast_test();
			break;
	}
	printf("Return value: %d\n", ret);
	return ret;
}
