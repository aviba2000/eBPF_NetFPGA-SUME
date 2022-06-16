#ifndef __PARSING_HELPERS__
#define __PARSING_HELPERS__

#include <linux/if_ether.h>
#include <linux/ip.h>
#include <stdint.h>
#include <stdio.h>
#include <endian.h>

#include "../def/DataTypes.h"

int parse_ethhdr(int *pos, const uint8_t data[MAX_PACKET_LEN], union ethhdr_byte_data *ethhdr, int size);

unsigned short parse_vlanhdr(int *pos, const uint8_t data[MAX_PACKET_LEN], union vlanhdr_byte_data *vlanhdr, int size);

int parse_iphdr(int *pos, const uint8_t data[MAX_PACKET_LEN], union iphdr_byte_data *iphdr, int size);

int parse_ipv6hdr(int *pos, const uint8_t data[MAX_PACKET_LEN], union ipv6hdr_byte_data *ipv6hdr, int size);

int parse_icmphdr(int *pos, const uint8_t data[MAX_PACKET_LEN], union icmphdr_byte_data *icmph, int size);
#endif
