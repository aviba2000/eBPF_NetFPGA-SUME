#include "xdp_prog_kern.h"

#define IPPROTO_ICMP 1

void copy_array2(const uint8_t data[MAX_PACKET_LEN], uint8_t data_tmp[MAX_PACKET_LEN], uint8_t data_tmp2[MAX_PACKET_LEN], unsigned int size){
	int i;
	uint8_t aux;
	int sum;
	for (i = 0; i < MAX_PACKET_LEN; i++) {
		if (i < size) {
			aux = data[i];
			data_tmp[i] = aux;
			data_tmp2[i] = aux;
		}
	}
}

int parsing_task(const uint8_t data_tmp[MAX_PACKET_LEN], int size, union ethhdr_byte_data *eth,
		union iphdr_byte_data *iph, union icmphdr_byte_data *icmph, int *action) {
	int pos = 0;

	int nh_type = parse_ethhdr(&pos, data_tmp, eth, size);
	if (nh_type == -1) return -5;

	if (nh_type == htobe16(ETH_P_IP)) {
		nh_type = parse_iphdr(&pos, data_tmp, iph, size);
		if (nh_type == -1) return -4;
	} else return -1;

	if (nh_type == IPPROTO_ICMP) {
		nh_type = parse_icmphdr(&pos, data_tmp, icmph, size);
	} else return -3;

	*action = XDP_PASS;

	return 0;
}

int writing_task(uint8_t data_out[MAX_PACKET_LEN], const uint8_t data_tmp2[MAX_PACKET_LEN], int size,
		const union ethhdr_byte_data *eth, const union iphdr_byte_data *iph, const union icmphdr_byte_data *icmph) {
	write_Eth_hdr(ETH_HLEN, data_out, eth);

	write_IPv4_hdr(ETH_HLEN+IPv4_BASE_LEN, data_out, iph);

	write_ICMP_hdr(ETH_HLEN+IPv4_BASE_LEN+ICMP_HDR_LEN, data_out, icmph);

	write_remaining_packet(ETH_HLEN+IPv4_BASE_LEN+ICMP_HDR_LEN, data_tmp2, data_out, size);

	return 0;
}


int eBPF_Ping_OPTIMIZED(const uint8_t data_in[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], unsigned int size_in, unsigned int *size_out) {
	#pragma HLS pipeline // #pragma HLS pipeline II=1 CRASHES!

	#ifndef __SYNTHESIS__
		uint8_t *data_tmp = (uint8_t *) malloc(MAX_PACKET_LEN);
		uint8_t *data_tmp2 = (uint8_t *) malloc(MAX_PACKET_LEN);
	#else
		uint8_t data_tmp[MAX_PACKET_LEN];
		uint8_t data_tmp2[MAX_PACKET_LEN];
	#endif

	#pragma HLS array_partition variable=data_tmp complete
	#pragma HLS array_partition variable=data_tmp2 complete

	int size_cpy = size_in;

	int s_out = size_cpy;

	int action = XDP_DROP;


	copy_array2(data_in, data_tmp, data_tmp2, size_cpy);


	union ethhdr_byte_data eth;
	union iphdr_byte_data iph;
	union icmphdr_byte_data icmph;

	int err = parsing_task(data_tmp, size_cpy, &eth, &iph, &icmph, &action);
	if (err < 0) return err;

	if (htobe16(icmph.icmph.un.echo.sequence)%2 == 0) {
		s_out = 0;
		action = -2;
	} else {
		err = writing_task(data_out, data_tmp2, size_cpy, &eth, &iph, &icmph);
	}

	#ifndef __SYNTHESIS__
		free(data_tmp);
		free(data_tmp2);
	#endif

	*size_out = s_out;

	return action;
}

void PING_program(hls::stream<trans_pkt >& data_in, hls::stream<trans_pkt >& data_out) {
	#pragma HLS pipeline // Otherwise there are one/two cycles of overhead in the optimized version
	#pragma HLS INTERFACE mode=axis port=data_in
	#pragma HLS INTERFACE mode=axis port=data_out
	unsigned int real_size = 0;
	trans_pkt t2;
	int position = 0;

	ap_uint<128> Tuser[MAXIMUM_CHUNKS_DATA];
	ap_uint<32> Tkeep[MAXIMUM_CHUNKS_DATA];

	uint8_t data_i[MAX_PACKET_LEN];
	uint8_t data_o[MAX_PACKET_LEN];
	#pragma HLS array_partition variable=data_i cyclic factor=32
	#pragma HLS array_partition variable=data_o cyclic factor=32

	bool end = false;

	for (int i = 0; i < MAX_PACKET_LEN; i+=PACK_STREAM_TO_DATA) {

		if (!end) {
			t2 = data_in.read();

			data_i[real_size++] = (uint8_t)(t2.data & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 8) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 16) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 24) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 32) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 40) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 48) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 56) & 0xFF);

			data_i[real_size++] = (uint8_t)((t2.data >> 64) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 72) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 80) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 88) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 96) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 104) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 112) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 120) & 0xFF);

			data_i[real_size++] = (uint8_t)((t2.data >> 128) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 136) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 144) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 152) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 160) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 168) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 176) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 184) & 0xFF);

			data_i[real_size++] = (uint8_t)((t2.data >> 192) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 200) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 208) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 216) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 224) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 232) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 240) & 0xFF);
			data_i[real_size++] = (uint8_t)((t2.data >> 248) & 0xFF);

			Tuser[position] = t2.user;
			Tkeep[position] = t2.keep;

			position++;
			end = t2.last;
		}
	}

	unsigned int new_size;
	int action = eBPF_Ping_OPTIMIZED(data_i, data_o, real_size, &new_size);

	int pos = 0;
	position = 0;

	end = false;

	for (int i = 0; i < MAX_PACKET_LEN; i+=PACK_STREAM_TO_DATA) {
		ap_uint<256> aux;
		if (i < new_size) {
			pos = i;

			ap_uint<8> a = data_o[pos++] & 0xFF;
			ap_uint<16> b = data_o[pos++] & 0xFF;
			ap_uint<24> c = data_o[pos++] & 0xFF;
			ap_uint<32> d = data_o[pos++] & 0xFF;
			ap_uint<40> e = data_o[pos++] & 0xFF;
			ap_uint<48> f = data_o[pos++] & 0xFF;
			ap_uint<56> g = data_o[pos++] & 0xFF;
			ap_uint<64> h = data_o[pos++] & 0xFF;
			unsigned long long bits64_0 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

			a = data_o[pos++] & 0xFF;
			b = data_o[pos++] & 0xFF;
			c = data_o[pos++] & 0xFF;
			d = data_o[pos++] & 0xFF;
			e = data_o[pos++] & 0xFF;
			f = data_o[pos++] & 0xFF;
			g = data_o[pos++] & 0xFF;
			h = data_o[pos++] & 0xFF;
			unsigned long long bits64_1 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

			a = data_o[pos++] & 0xFF;
			b = data_o[pos++] & 0xFF;
			c = data_o[pos++] & 0xFF;
			d = data_o[pos++] & 0xFF;
			e = data_o[pos++] & 0xFF;
			f = data_o[pos++] & 0xFF;
			g = data_o[pos++] & 0xFF;
			h = data_o[pos++] & 0xFF;
			unsigned long long bits64_2 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

			a = data_o[pos++] & 0xFF;
			b = data_o[pos++] & 0xFF;
			c = data_o[pos++] & 0xFF;
			d = data_o[pos++] & 0xFF;
			e = data_o[pos++] & 0xFF;
			f = data_o[pos++] & 0xFF;
			g = data_o[pos++] & 0xFF;
			h = data_o[pos++] & 0xFF;
			unsigned long long bits64_3 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

			ap_uint<128> first_sec = (ap_uint<64>(bits64_1), ap_uint<64>(bits64_0));
			ap_uint<128> third_fourth = (ap_uint<64>(bits64_3), ap_uint<64>(bits64_2));

			aux = (third_fourth, first_sec);

			t2.data = aux;
			t2.user = Tuser[position];
			t2.keep = Tkeep[position];
			t2.last = !(i < new_size-PACK_STREAM_TO_DATA-1);
			data_out.write(t2);
			position++;
		}
	}
}

int eBPF_Ping_NON_OPTIMIZED(const uint8_t data_in[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], unsigned int size_in, unsigned int *size_out) {
	int s_out = size_in;
	int action = XDP_DROP;

	union ethhdr_byte_data eth;
	union iphdr_byte_data iph;
	union icmphdr_byte_data icmph;

	int err = parsing_task(data_in, size_in, &eth, &iph, &icmph, &action);
	if (err < 0) return err;

	if (htobe16(icmph.icmph.un.echo.sequence)%2 == 0) {
		s_out = 0;
		action = -2;
	} else {
		err = writing_task(data_out, data_in, size_in, &eth, &iph, &icmph);
	}

	*size_out = s_out;

	return action;
}

void PING_program_NON_OPTIMIZED(hls::stream<trans_pkt >& data_in, hls::stream<trans_pkt >& data_out) {
	#pragma HLS INTERFACE mode=axis port=data_in
	#pragma HLS INTERFACE mode=axis port=data_out
	unsigned int real_size = 0;
	trans_pkt t2;
	int position = 0;

	ap_uint<128> Tuser[MAXIMUM_CHUNKS_DATA];
	ap_uint<32> Tkeep[MAXIMUM_CHUNKS_DATA];

	uint8_t data_i[MAX_PACKET_LEN];
	uint8_t data_o[MAX_PACKET_LEN];

	bool end = false;

	while (!end) {
		t2 = data_in.read();

		data_i[real_size++] = (uint8_t)(t2.data & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 8) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 16) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 24) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 32) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 40) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 48) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 56) & 0xFF);

		data_i[real_size++] = (uint8_t)((t2.data >> 64) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 72) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 80) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 88) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 96) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 104) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 112) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 120) & 0xFF);

		data_i[real_size++] = (uint8_t)((t2.data >> 128) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 136) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 144) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 152) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 160) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 168) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 176) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 184) & 0xFF);

		data_i[real_size++] = (uint8_t)((t2.data >> 192) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 200) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 208) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 216) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 224) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 232) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 240) & 0xFF);
		data_i[real_size++] = (uint8_t)((t2.data >> 248) & 0xFF);

		Tuser[position] = t2.user;
		Tkeep[position] = t2.keep;

		position++;
		end = t2.last;
	}

	unsigned int new_size;
	int action = eBPF_Ping_NON_OPTIMIZED(data_i, data_o, real_size, &new_size);

	int pos = 0;
	position = 0;

	end = false;

	int i = 0;
	while (i < new_size) {
		ap_uint<256> aux;
		pos = i;

		ap_uint<8> a = data_o[pos++] & 0xFF;
		ap_uint<16> b = data_o[pos++] & 0xFF;
		ap_uint<24> c = data_o[pos++] & 0xFF;
		ap_uint<32> d = data_o[pos++] & 0xFF;
		ap_uint<40> e = data_o[pos++] & 0xFF;
		ap_uint<48> f = data_o[pos++] & 0xFF;
		ap_uint<56> g = data_o[pos++] & 0xFF;
		ap_uint<64> h = data_o[pos++] & 0xFF;
		unsigned long long bits64_0 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

		a = data_o[pos++] & 0xFF;
		b = data_o[pos++] & 0xFF;
		c = data_o[pos++] & 0xFF;
		d = data_o[pos++] & 0xFF;
		e = data_o[pos++] & 0xFF;
		f = data_o[pos++] & 0xFF;
		g = data_o[pos++] & 0xFF;
		h = data_o[pos++] & 0xFF;
		unsigned long long bits64_1 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

		a = data_o[pos++] & 0xFF;
		b = data_o[pos++] & 0xFF;
		c = data_o[pos++] & 0xFF;
		d = data_o[pos++] & 0xFF;
		e = data_o[pos++] & 0xFF;
		f = data_o[pos++] & 0xFF;
		g = data_o[pos++] & 0xFF;
		h = data_o[pos++] & 0xFF;
		unsigned long long bits64_2 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

		a = data_o[pos++] & 0xFF;
		b = data_o[pos++] & 0xFF;
		c = data_o[pos++] & 0xFF;
		d = data_o[pos++] & 0xFF;
		e = data_o[pos++] & 0xFF;
		f = data_o[pos++] & 0xFF;
		g = data_o[pos++] & 0xFF;
		h = data_o[pos++] & 0xFF;
		unsigned long long bits64_3 = (h<<56)+(g<<48)+(f<<40)+(e<<32)+(d<<24)+(c<<16)+(b<<8)+a;

		ap_uint<128> first_sec = (ap_uint<64>(bits64_1), ap_uint<64>(bits64_0));
		ap_uint<128> third_fourth = (ap_uint<64>(bits64_3), ap_uint<64>(bits64_2));

		aux = (third_fourth, first_sec);

		t2.data = aux;
		t2.user = Tuser[position];
		t2.keep = Tkeep[position];
		t2.last = !(i < new_size-PACK_STREAM_TO_DATA-1);
		data_out.write(t2);
		position++;
		i+=PACK_STREAM_TO_DATA;
	}
}
