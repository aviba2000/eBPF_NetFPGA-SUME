#ifndef __WRITING_HELPERS__
#define __WRITING_HELPERS__

#include <linux/if_ether.h>
#include <linux/ip.h>
#include <stdint.h>
#include <stdio.h>
#include <endian.h>

#include "../def/DataTypes.h"

int write_Eth_hdr(int pos, uint8_t data[MAX_PACKET_LEN], const union ethhdr_byte_data *ethhdr);
int write_VLAN_hdr(int pos, uint8_t data[MAX_PACKET_LEN], const union vlanhdr_byte_data *vlanhdr);
int write_IPv4_hdr(int pos, uint8_t data[MAX_PACKET_LEN], const union iphdr_byte_data *iphdr);
int write_IPv6_hdr(int pos, uint8_t data[MAX_PACKET_LEN], const union ipv6hdr_byte_data *ipv6hdr);
int write_ICMP_hdr(int pos, uint8_t data[MAX_PACKET_LEN], const union icmphdr_byte_data *icmph);
int write_remaining_packet(int pos, const uint8_t data[ETH_TOTAL_LEN], uint8_t data_out[MAX_PACKET_LEN], int size);
int write_remaining_packet_2_indexes(int pos_data, int pos, const uint8_t data[MAX_PACKET_LEN], uint8_t data_out[MAX_PACKET_LEN], int size);

#endif
