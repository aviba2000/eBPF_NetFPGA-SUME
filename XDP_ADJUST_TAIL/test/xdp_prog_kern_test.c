#include <stdio.h>
#include <linux/bpf.h>
#include "../src/xdp_prog_kern.h"

#define BIG_SIZE_IN 242
#define BIG_SIZE_OUT 126

uint8_t in_big_pack[242] = {66,167,140,216,215,172,178,58,160,166,145,14,8,0,69,0,0,228,217,82,64,0,64,1,74,174,10,11,1,2,10,11,1,1,8,0,101,123,171,106,0,3,6,225,114,98,159,172,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199};
uint8_t out_big_pack[126] = {178,58,160,166,145,14,66,167,140,216,215,172,8,0,69,0,0,112,0,0,0,0,64,1,100,117,10,11,1,1,10,11,1,2,3,4,71,189,145,14,0,186,69,0,0,228,217,82,64,0,64,1,74,174,10,11,1,2,10,11,1,1,8,0,101,123,171,106,0,3,6,225,114,98,159,172,7,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};

void print_array(uint8_t array[], int size) {
	int i;
	for (i = 0; i < size; i++) {
		printf("%d", (int) array[i]);
		if (i == size-1) printf("\n");
		else printf(",");
	}
}

enum Testing_function {
	all,
	all_x2,
	big
};

enum Testing_function func = all_x2;

long rxcnt[256];


int big_case() {
	//Big case: the packet is too big
	printf("Big case scenario: the packet is too big, so it will be shrunk\n");
	uint8_t data[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < BIG_SIZE_IN; i++) data[i] = in_big_pack[i];
	for (; i < MAX_PACKET_LEN; i++) data[i] = 0;

	uint8_t data_out[MAX_PACKET_LEN];
	for (i = 0; i < MAX_PACKET_LEN; i++) data_out[i] = 0;

	rxcnt[15] = 1;

	unsigned int size_func_in = BIG_SIZE_IN;
	unsigned int size_func;

	int res = xdp_prog1(data, data_out, size_func_in, &size_func, rxcnt);

	int result = !(res == XDP_TX);

	if (size_func != BIG_SIZE_OUT) result++;

	__be16 MTU = htobe16(max_pcktsz - sizeof(struct ethhdr));
	__u8 MTU_low = MTU & 0xFF;
	__u8 MTU_high = MTU >> 8;
	printf("MTU high = %d\n", MTU);
	out_big_pack[40] = MTU_low; // This value depends on the value of max packet size
	out_big_pack[41] = MTU_high; // This value depends on the value of max packet size
	// MTU values also affect the checksum of the packet...

	for (i = 0; i < BIG_SIZE_OUT; i++) {
		if (data_out[i] != out_big_pack[i]) {
			if (i != 36 && i != 37)	result++; // We do not check the checksum as it depends on the MAX_PACKET_LEN variable, which is continuously changing for testing purposes
			printf("Diff found at %d, data_out[i] = %d and out_val[i] = %d\n", i, (int) data_out[i], (int) out_big_pack[i]);
		}
	}

	printf("DATA_IN  values: ");
	print_array(in_big_pack, BIG_SIZE_IN);
	printf("DATA_OUT values: ");
	print_array(data_out, BIG_SIZE_OUT);
	printf("Correct  values: ");
	print_array(out_big_pack, BIG_SIZE_OUT);

	printf("Finished big case scenario: the packet is too big, so it will be shrunk\n");

	if (!result) return 0;
	return -1;
}


int main() {
	int ret = 1;
	switch(func) {
		case all:
			ret = big_case();
			break;
		case all_x2:
			ret = big_case();
			if (ret < 0) break;
			ret += big_case();
			break;
		case big:
			ret = big_case();
			break;
	}
	printf("Return value: %d\n", ret);
	return ret;
}
