#include "function_helpers.h"

uint16_t IPV4Checksum_non_optimized(uint16_t *pkt, uint16_t hlen, uint16_t init) {
    uint32_t csum = init;

    csum += pkt[0] + pkt[1] + pkt[2] + pkt[3] + pkt[4] + pkt[6] + pkt[7] +
        pkt[8] + pkt[9];

    hlen -= 20;
    pkt += 10;

    if (hlen == 0) {
        ;
    } else if (hlen == 4) {
        csum += pkt[0] + pkt[1];
    } else if (hlen == 8) {
        csum += pkt[0] + pkt[1] + pkt[2] + pkt[3];
    } else if (hlen == 12) {
        csum += pkt[0] + pkt[1] + pkt[2] + pkt[3] + pkt[4] + pkt[5];
    } else if (hlen == 16) {
        csum += pkt[0] + pkt[1] + pkt[2] + pkt[3] + pkt[4] + pkt[5] + pkt[6] +
            pkt[7];
    } else if (hlen == 20) {
        csum += pkt[0] + pkt[1] + pkt[2] + pkt[3] + pkt[4] + pkt[5] + pkt[6] +
            pkt[7] + pkt[8] + pkt[9];
    } else if (hlen == 24) {
        csum += pkt[0] + pkt[1] + pkt[2] + pkt[3] + pkt[4] + pkt[5] + pkt[6] +
            pkt[7] + pkt[8] + pkt[9] + pkt[10] + pkt[11];
    } else if (hlen == 28) {
        csum += pkt[0] + pkt[1] + pkt[2] + pkt[3] + pkt[4] + pkt[5] + pkt[6] +
            pkt[7] + pkt[8] + pkt[9] + pkt[10] + pkt[11] + pkt[12] + pkt[13];
    } else if (hlen == 32) {
        csum += pkt[0] + pkt[1] + pkt[2] + pkt[3] + pkt[4] + pkt[5] + pkt[6] +
            pkt[7] + pkt[8] + pkt[9] + pkt[10] + pkt[11] + pkt[12] + pkt[13] +
            pkt[14] + pkt[15];
    } else if (hlen == 36) {
        csum += pkt[0] + pkt[1] + pkt[2] + pkt[3] + pkt[4] + pkt[5] + pkt[6] +
            pkt[7] + pkt[8] + pkt[9] + pkt[10] + pkt[11] + pkt[12] + pkt[13] +
            pkt[14] + pkt[15] + pkt[16] + pkt[17];
    } else if (hlen == 40) {
        csum += pkt[0] + pkt[1] + pkt[2] + pkt[3] + pkt[4] + pkt[5] + pkt[6] +
            pkt[7] + pkt[8] + pkt[9] + pkt[10] + pkt[11] + pkt[12] + pkt[13] +
            pkt[14] + pkt[15] + pkt[16] + pkt[17] + pkt[18] + pkt[19];
    }

    csum = (csum >> 16) + (csum & 0x0000FFFF);
    csum += (csum >> 16);

    return (uint16_t) ~csum;
}

uint16_t Checksum_non_optimized(uint16_t *ptr, int len, unsigned int acc) {
      long sum = acc;  // assume 32 bit long, 16 bit short

      while(len > 1){
        sum += *ptr++;
        if(sum & 0x80000000)   // if high order bit set, fold
          sum = (sum & 0xFFFF) + (sum >> 16);
        len -= 2;
      }

      if(len)       // take care of left over byte
        sum += *((uint16_t*)ptr);

      while(sum>>16)
        sum = (sum & 0xFFFF) + (sum >> 16);

      return ~sum;

}

__always_inline uint16_t Checksum_optimized(uint16_t *ptr, int len, unsigned int acc) {
      long sum = acc;

      int i;
      for (i = 0; i < 92; i++) { // #define ICMP_TOOBIG_PAYLOAD_SIZE 92
    	  if (len > 1) {
    		sum += *ptr++;
			if(sum & 0x80000000)
			  sum = (sum & 0xFFFF) + (sum >> 16);
			len -= 2;
    	  }
      }

      if(len) sum += *((uint16_t*)ptr);

      sum = (sum & 0xFFFF) + (sum >> 16);
      sum = (sum & 0xFFFF) + (sum >> 16);

      return ~sum;

}

uint16_t Checksum_8bits_optimized(uint8_t *ptr, int len, unsigned int acc) {
/*
You basically treat all data as WORDs (16-bit integers) and add them all togethor while recording carry outs.
At the end of the checksum, add all the accumulated carry outs back in (as if they were more 16 bit chunk of data).
Do this once more to prevent the carry outs from causing another unhandled carry out.
Finally, take the one's complement of the result (invert all bits) and you're done.
Source: https://www.microchip.com/forums/m223760.aspx
*/
      long sum = acc;

      int i, loop_size = 92>>1;
      for (i = 0; i < loop_size; i++) {
    	  if (len > 1) {
    		sum += (*ptr++ + ((*ptr++)<<8));
    		if(sum & 0x80000000)
			  sum = (sum & 0xFFFF) + (sum >> 16);
			len -= 2;
    	  }
      }

      if(len) sum += (*ptr++ + ((*ptr++)<<8));

      sum = (sum & 0xFFFF) + (sum >> 16);
      sum = (sum & 0xFFFF) + (sum >> 16);

      return ~sum;
}


uint16_t IPV4Checksum(uint16_t *pkt, uint16_t hlen, uint16_t init) {
	return IPV4Checksum_non_optimized(pkt, hlen, init);
}

uint16_t Checksum(uint16_t *ptr, int len, unsigned int acc) {
	if (!OPTIMIZATION) return Checksum_non_optimized(ptr, len, acc);
	else return Checksum_optimized(ptr, len, acc);
}

