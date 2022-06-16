#include "../src/function_helpers.h"
#include "function_helpers_test.h"

enum Testing_function {
	all,
	IPV4Checksum_func
};

enum Testing_function test_function = all;

int IPV4Checksum_test() {
	// Test 1: Testing the checksum calculation
	printf("Starting IPV4Checksum Test 1: Testing the checksum calculation\n");
	uint8_t in_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,126,154,64,0,64,1,163,246,128,138,129,98,10,11,2,1,8,0,163,221,133,158,0,2,122,81,42,98,50,203,12,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	uint8_t out_val[98] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,126,154,64,0,64,1,174,22,128,138,129,98,10,11,2,1,8,0,163,221,133,158,0,2,122,81,42,98,50,203,12,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
	int i, pos = 14;
	union iphdr_byte_data iph;
	for (i = 0; i < 20; i++) iph.bytes[i] = in_val[pos+i];
	iph.iph.check = IPV4Checksum(&(iph.bytes16[0]), htobe16(iph.iph.tot_len), 0);
	for (i = 0; i < 20; i++) if (iph.bytes[i] != out_val[pos+i]) return -1;
	printf("IPV4Checksum Test 1 completed with success!\n");
	return 0;
}

int main_function() {
	// All the testing functions return 0 if everything is OK, any another value (< 0) otherwise.
	int ret = 1;
	switch(test_function) {
		case all:
			ret = IPV4Checksum_test();
			break;
		case IPV4Checksum_func:
			ret = IPV4Checksum_test();
			break;
	}
	printf("Return value: %d\n", ret);
	return ret;
}
