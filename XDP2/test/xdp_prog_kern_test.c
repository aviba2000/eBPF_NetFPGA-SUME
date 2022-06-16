#include <stdio.h>
#include <linux/bpf.h>
#include "../src/xdp_prog_kern.h"

#define SIZE_best 43
#define SIZE_medium 47
#define SIZE_worst 51

uint8_t in_val_best[SIZE_best] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,29,220,51,64,0,64,17,70,132,10,11,2,2,10,11,2,1,169,143,7,208,0,9,24,51,10};
uint8_t out_val_best[SIZE_best] = {246,32,182,253,7,73,174,226,157,153,63,15,8,0,69,0,0,29,220,51,64,0,64,17,70,132,10,11,2,2,10,11,2,1,169,143,7,208,0,9,24,51,10};

uint8_t in_val_medium[SIZE_medium] = {174,226,157,153,63,15,246,32,182,253,7,73,129,0,0,1,8,0,69,0,0,29,220,51,64,0,64,17,70,132,10,11,2,2,10,11,2,1,169,143,7,208,0,9,24,51,10};
uint8_t out_val_medium[SIZE_medium] = {246,32,182,253,7,73,174,226,157,153,63,15,129,0,0,1,8,0,69,0,0,29,220,51,64,0,64,17,70,132,10,11,2,2,10,11,2,1,169,143,7,208,0,9,24,51,10};

uint8_t in_val_worst[SIZE_worst] = {174,226,157,153,63,15,246,32,182,253,7,73,129,0,0,1,129,0,0,1,8,0,69,0,0,29,220,51,64,0,64,17,70,132,10,11,2,2,10,11,2,1,169,143,7,208,0,9,24,51,10};
uint8_t out_val_worst[SIZE_worst] = {246,32,182,253,7,73,174,226,157,153,63,15,129,0,0,1,129,0,0,1,8,0,69,0,0,29,220,51,64,0,64,17,70,132,10,11,2,2,10,11,2,1,169,143,7,208,0,9,24,51,10};


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
	best,
	medium,
	worst
};

enum Testing_function func = all;

long rxcnt[256];


int best_case() {
	// Best case: NO VLAN
	printf("Best case scenario: No VLAN header\n");
	uint8_t data[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < SIZE_best; i++) data[i] = in_val_best[i];
	for (; i < MAX_PACKET_LEN; i++) data[i] = 0;

	uint8_t data_out[MAX_PACKET_LEN];
	for (i = 0; i < MAX_PACKET_LEN; i++) data_out[i] = 0;

	rxcnt[15] = 1;

	unsigned int size_out;

	int res = xdp2(data, data_out, SIZE_best, &size_out, rxcnt);

	int result = !(res == XDP_TX);

	for (i = 0; i < SIZE_best; i++) {
		if (data_out[i] != out_val_best[i]) {
			result++;
			printf("Diff found at %d, data_out[i] = %d and out_val[i] = %d\n", i, (int) data_out[i], (int) out_val_best[i]);
		}
	}

	for (; i < MAX_PACKET_LEN; i++) {
		if (data_out[i] != 0) result++;
	}

	print_array(data_out, SIZE_best);
	print_array(out_val_best, SIZE_best);

	printf("Finished best case scenario: No VLAN header\n");

	if (!result) return 0;
	return -1;
}

int medium_case() {
	// Medium case: ONE VLAN header
	printf("Medium case scenario: ONE VLAN header\n");
	uint8_t data[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < SIZE_medium; i++) data[i] = in_val_medium[i];
	for (; i < MAX_PACKET_LEN; i++) data[i] = 0;

	uint8_t data_out[MAX_PACKET_LEN];
	for (i = 0; i < MAX_PACKET_LEN; i++) data_out[i] = 0;

	rxcnt[15] = 1;

	unsigned int size_out;

	int res = xdp2(data, data_out, SIZE_medium, &size_out, rxcnt);

	int result = !(res == XDP_TX);

	for (i = 0; i < SIZE_medium; i++) {
		if (data_out[i] != out_val_medium[i]) {
			result++;
			printf("Diff found at %d, data_out[i] = %d and out_val[i] = %d\n", i, (int) data_out[i], (int) out_val_medium[i]);
		}
	}

	for (; i < MAX_PACKET_LEN; i++) {
		if (data_out[i] != 0) result++;
	}

	print_array(data_out, SIZE_medium);
	print_array(out_val_medium, SIZE_medium);

	printf("Finished medium case scenario: ONE VLAN header\n");

	if (!result) return 0;
	return -1;
}

int worst_case() {
	// Worst case: TWO VLAN header
	printf("Worst case scenario: TWO VLAN header\n");
	uint8_t data[MAX_PACKET_LEN];
	int i;
	for (i = 0; i < SIZE_worst; i++) data[i] = in_val_worst[i];
	for (; i < MAX_PACKET_LEN; i++) data[i] = 0;

	uint8_t data_out[MAX_PACKET_LEN];
	for (i = 0; i < MAX_PACKET_LEN; i++) data_out[i] = 0;

	rxcnt[15] = 1;

	unsigned int size_out;

	int res = xdp2(data, data_out, SIZE_worst, &size_out, rxcnt);

	int result = !(res == XDP_TX);

	for (i = 0; i < SIZE_worst; i++) {
		if (data_out[i] != out_val_worst[i]) {
			result++;
			printf("Diff found at %d, data_out[i] = %d and out_val[i] = %d\n", i, (int) data_out[i], (int) out_val_worst[i]);
		}
	}

	for (; i < MAX_PACKET_LEN; i++) {
		if (data_out[i] != 0) result++;
	}

	print_array(data_out, SIZE_worst);
	print_array(out_val_worst, SIZE_worst);

	printf("Finished worst case scenario: TWO VLAN header\n");

	if (!result) return 0;
	return -1;
}

int main() {
	int ret = 1;
	switch(func) {
		case all:
			ret = best_case();
			if (ret < 0) break;
			ret += medium_case();
			if (ret < 0) break;
			ret += worst_case();
			break;
		case best:
			ret = best_case();
			break;
		case medium:
			ret = medium_case();
			break;
		case worst:
			ret = worst_case();
			break;
	}
	printf("Return value: %d\n", ret);
	return ret;
}
