#include "../src/xdp_prog_kern.h"

#define SIZE 128

uint8_t packet_even[SIZE] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,126,154,64,0,64,1,163,246,10,11,2,2,10,11,2,1,8,0,163,221,133,158,0,2,122,81,42,98,50,203,12,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

uint8_t packet_odd[SIZE] = {174,226,157,153,63,15,246,32,182,253,7,73,8,0,69,0,0,84,126,154,64,0,64,1,163,246,10,11,2,2,10,11,2,1,8,0,163,221,133,158,0,1,122,81,42,98,50,203,12,0,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


int test_odd(int packet_len) {
	int ret = 0;
	hls::stream<trans_pkt > data_INPUT;
	hls::stream<trans_pkt > data_OUTPUT;

	trans_pkt t2;
	trans_pkt aux;

	int orig_i;

    for (int i = 0; i < packet_len; ) {
		orig_i = i;
    	if (i < packet_len-PACK_STREAM_TO_DATA-1) {
			aux.last = 0;
		} else {
			aux.last = 1;
		}

		ap_uint<8> a = packet_odd[i++] & 0xFF;
		ap_uint<16> b = packet_odd[i++] & 0xFF;
		ap_uint<24> c = packet_odd[i++] & 0xFF;
		ap_uint<32> d = packet_odd[i++] & 0xFF;
		ap_uint<40> e = packet_odd[i++] & 0xFF;
		ap_uint<48> f = packet_odd[i++] & 0xFF;
		ap_uint<56> g = packet_odd[i++] & 0xFF;
		ap_uint<64> h = packet_odd[i++] & 0xFF;
		unsigned long long bits64_0 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

		a = packet_odd[i++] & 0xFF;
		b = packet_odd[i++] & 0xFF;
		c = packet_odd[i++] & 0xFF;
		d = packet_odd[i++] & 0xFF;
		e = packet_odd[i++] & 0xFF;
		f = packet_odd[i++] & 0xFF;
		g = packet_odd[i++] & 0xFF;
		h = packet_odd[i++] & 0xFF;
		unsigned long long bits64_1 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

		a = packet_odd[i++] & 0xFF;
		b = packet_odd[i++] & 0xFF;
		c = packet_odd[i++] & 0xFF;
		d = packet_odd[i++] & 0xFF;
		e = packet_odd[i++] & 0xFF;
		f = packet_odd[i++] & 0xFF;
		g = packet_odd[i++] & 0xFF;
		h = packet_odd[i++] & 0xFF;
		unsigned long long bits64_2 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

		a = packet_odd[i++] & 0xFF;
		b = packet_odd[i++] & 0xFF;
		c = packet_odd[i++] & 0xFF;
		d = packet_odd[i++] & 0xFF;
		e = packet_odd[i++] & 0xFF;
		f = packet_odd[i++] & 0xFF;
		g = packet_odd[i++] & 0xFF;
		h = packet_odd[i++] & 0xFF;
		unsigned long long bits64_3 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

		ap_uint<128> first_sec = (ap_uint<64>(bits64_1), ap_uint<64>(bits64_0));
		ap_uint<128> third_fourth = (ap_uint<64>(bits64_3), ap_uint<64>(bits64_2));

		ap_uint<256> data256 = (third_fourth, first_sec);

		aux.data = data256;
		aux.user = orig_i;
		aux.keep = orig_i+10;
		data_INPUT.write(aux);
	}

    PING_program(data_INPUT, data_OUTPUT);

	uint8_t buffer[PACK_STREAM_TO_DATA];

	int pos, pos_meta = 0;

	for (int i = 0; i < packet_len;) {
		t2 = data_OUTPUT.read();
		orig_i = i;

		pos = 0;
		buffer[pos++] = (uint8_t)(t2.data & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 8) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 16) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 24) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 32) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 40) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 48) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 56) & 0xFF);

		buffer[pos++] = (uint8_t)((t2.data >> 64) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 72) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 80) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 88) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 96) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 104) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 112) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 120) & 0xFF);

		buffer[pos++] = (uint8_t)((t2.data >> 128) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 136) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 144) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 152) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 160) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 168) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 176) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 184) & 0xFF);

		buffer[pos++] = (uint8_t)((t2.data >> 192) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 200) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 208) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 216) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 224) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 232) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 240) & 0xFF);
		buffer[pos++] = (uint8_t)((t2.data >> 248) & 0xFF);

		for (int w = 0; w < PACK_STREAM_TO_DATA; w++) {
			if (buffer[w] != (uint8_t) packet_odd[i]) {
				printf("ERROR: data_out = %d, when it should be %d at w = %d\n", (int) buffer[w], packet_odd[i], w);
				ret++;
			}
			i++;
		}

		if (t2.user != orig_i) {
			printf("ERROR: tuser = %d, when it should be %d\n", (int) t2.user, orig_i);
			ret++;
		}

		if (t2.keep != orig_i+10) {
			printf("ERROR: tkeep = %d, when it should be %d\n", (int) t2.keep, orig_i);
			ret++;
		}

		if (!(i < packet_len-1)) {
			if (!t2.last) {
				printf("ERROR: tlast = 0, when it should be 1 at i=%d\n", i);
				ret++;
			} else {
				printf("tlast = 1, at i=%d\n", i);
			}
		}
	}
	return ret;
}

int test_even(int packet_len) {
	int ret = 0;
	hls::stream<trans_pkt > data_INPUT;
	hls::stream<trans_pkt > data_OUTPUT;

	trans_pkt t2;
	trans_pkt aux;

	int orig_i;

    for (int i = 0; i < packet_len; ) {
		orig_i = i;
    	if (i < packet_len-PACK_STREAM_TO_DATA-1) {
			aux.last = 0;
		} else {
			aux.last = 1;
		}

		ap_uint<8> a = packet_even[i++] & 0xFF;
		ap_uint<16> b = packet_even[i++] & 0xFF;
		ap_uint<24> c = packet_even[i++] & 0xFF;
		ap_uint<32> d = packet_even[i++] & 0xFF;
		ap_uint<40> e = packet_even[i++] & 0xFF;
		ap_uint<48> f = packet_even[i++] & 0xFF;
		ap_uint<56> g = packet_even[i++] & 0xFF;
		ap_uint<64> h = packet_even[i++] & 0xFF;
		unsigned long long bits64_0 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

		a = packet_even[i++] & 0xFF;
		b = packet_even[i++] & 0xFF;
		c = packet_even[i++] & 0xFF;
		d = packet_even[i++] & 0xFF;
		e = packet_even[i++] & 0xFF;
		f = packet_even[i++] & 0xFF;
		g = packet_even[i++] & 0xFF;
		h = packet_even[i++] & 0xFF;
		unsigned long long bits64_1 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

		a = packet_even[i++] & 0xFF;
		b = packet_even[i++] & 0xFF;
		c = packet_even[i++] & 0xFF;
		d = packet_even[i++] & 0xFF;
		e = packet_even[i++] & 0xFF;
		f = packet_even[i++] & 0xFF;
		g = packet_even[i++] & 0xFF;
		h = packet_even[i++] & 0xFF;
		unsigned long long bits64_2 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

		a = packet_even[i++] & 0xFF;
		b = packet_even[i++] & 0xFF;
		c = packet_even[i++] & 0xFF;
		d = packet_even[i++] & 0xFF;
		e = packet_even[i++] & 0xFF;
		f = packet_even[i++] & 0xFF;
		g = packet_even[i++] & 0xFF;
		h = packet_even[i++] & 0xFF;
		unsigned long long bits64_3 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

		ap_uint<128> first_sec = (ap_uint<64>(bits64_1), ap_uint<64>(bits64_0));
		ap_uint<128> third_fourth = (ap_uint<64>(bits64_3), ap_uint<64>(bits64_2));

		ap_uint<256> data256 = (third_fourth, first_sec);

		aux.data = data256;
		aux.user = orig_i;
		aux.keep = orig_i+10;
		data_INPUT.write(aux);
	}

    PING_program(data_INPUT, data_OUTPUT);

	if (!data_OUTPUT.empty()) {
		printf("ERROR: data_out is NOT empty\n");
		ret++;
	}

	return ret;
}

int main() {
	int ret = 0;
	// The adapters require a size multiple of 32 (AXI4-Stream 256-bit wide)!
	// This testing file requires a size equal or less than 128 as the arrays used are small
	ret += test_odd(64);
	ret += test_odd(128);
	ret += test_even(64);
	ret += test_even(128);

	printf("Return value: %d\n", ret);
	return ret;
}
