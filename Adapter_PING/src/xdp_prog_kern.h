#ifndef XDP_PING_PROGRAM_
#define XDP_PING_PROGRAM_

#include <stdint.h>
#include <stdlib.h>
#include "../../00_Common_Libraries/def/DataTypes.h"
#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include <linux/bpf.h>

extern "C" {
	#include "../../00_Common_Libraries/src/parsing_helpers.h"
	#include "../../00_Common_Libraries/src/writing_helpers.h"
}

#define STREAM_DATA_SIZE 256
#define STREAM_META_DATA_SIZE 128
#define TKEEP_SIZE 32
#define PACK_STREAM_TO_DATA 32
#define MAXIMUM_CHUNKS_DATA 8 // MAX_PACKET_LEN*8/STREAM_DATA_SIZE

typedef ap_axiu<STREAM_DATA_SIZE, 128, 0, 0> trans_pkt;

void PING_program(hls::stream<trans_pkt >& data_in, hls::stream<trans_pkt >& data_out);

#endif
