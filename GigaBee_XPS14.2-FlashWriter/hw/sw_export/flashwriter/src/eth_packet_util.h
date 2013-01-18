/*
 * eth_packet_util.h
 *
 *  Created on: Dec 20, 2011
 *      Author: Ziga Lenarcic
 */

#ifndef ETH_PACKET_UTIL_H_
#define ETH_PACKET_UTIL_H_

#include <xil_io.h>
#include <string.h>

// define byteswaps for 'portable' code
#ifndef htons
#define htons(x) Xil_Htons(x)
#define ntohs(x) Xil_Ntohs(x)
#define htonl(x) Xil_Htonl(x)
#define ntohl(x) Xil_Ntohl(x)
#endif



#define IP_VERSION_4		0x0800	/* For IPV4, the Ethernet frame
					 * type/length field will have a
					 * value of 0x800
					 */
#define IP_HEADER_VERSION	0x04	/* For IPv4, the version entry in
					 * IP header is always 4
					 */
#define IP_HEADER_LEN_IN_WORDS	0x05	/* For our case, the header length
					 * is always 20 bytes (5 words
					 */
#define IP_HEADER_LENGTH	20	/* IP header length in bytes. Used
					 * as offset to kigure out the start
					 * of TCP header.
					 */
#define IP_HEADER_PROTOCOL_TCP	0x6	/* IP header protocol entry. For TCP
					 * packets, it is 6.
					 */
#define IP_HEADER_PROTOCOL_UDP	0x11	/* IP header protocol entry. For UDP
					 * packets, it is 17.
					 */
#define TCP_HEADER_LEN_IN_WORDS	0x5	/* For our case, the header length
					 * is always 20 bytes (5 words)
					 */
#define TCP_HEADER_LENGTH	20	/* IP header length in bytes. Used
					 * as offset to kigure out the start
					 * of TCP header.
					 */
#define UDP_HEADER_LENGTH 8 /* 8 bytes udp header */
#define ETH_HEADER_LEN 14
#define ETH_IP_HEADER_LENGTH 34
#define ETH_IP_UDP_HEADER_LENGTH 42
#define ETH_IP_TCP_HEADER_LENGTH 54

#define ARP_LENGTH 42



unsigned char TestMAC[6] = { 0x00, 0x0A, 0x35, 0x01, 0x02, 0x03 };
unsigned char BroadcastMAC[6]   = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


#ifndef QUAD_TO_IP
#define QUAD_TO_IP(a,b,c,d) (((u32)(a))         | \
							 (((u32)(b)) << 8)  | \
							 (((u32)(c)) << 16) | \
							 (((u32)(d)) << 24))
#endif

/*** PACKET LAYOUT ***/

typedef struct __attribute__ ((__packed__)) ether_hdr {
	unsigned char dst_mac[6];
	unsigned char src_mac[6];
	u16 ethertype;
} ether_hdr_t;

/* http://en.wikipedia.org/wiki/IPv4 */
typedef struct __attribute__ ((__packed__)) ip_hdr {
	u8 version_header_length; // should be 0x45 (version 4, 5*32bits=20bytes hlen)
	u8 dscp;
	u16 total_length; /* BIG ENDIAN ! */
	u16 identification;
	u16 flags;
	u8 time_to_live;
	u8 protocol;
	u16 header_checksum;
	u32 source_ip;
	u32 destination_ip;
} ip_hdr_t;

/* http://en.wikipedia.org/wiki/User_Datagram_Protocol */
typedef struct __attribute__ ((__packed__)) udp_hdr {
	u16 source_port; /* BIG ENDIAN ! */
	u16 destination_port;
	u16 length;
	u16 checksum;
} udp_hdr_t;

typedef struct __attribute__ ((__packed__)) ip_packet {
	ether_hdr_t ether_hdr;
	ip_hdr_t ip_hdr;
	unsigned char data[0];
} ip_packet_t;

typedef struct __attribute__ ((__packed__)) ip_udp_packet {
	ether_hdr_t ether_hdr;
	ip_hdr_t ip_hdr;
	udp_hdr_t udp_hdr;
	unsigned char data[0];
} ip_udp_packet_t;


typedef struct __attribute__ ((__packed__)) arp_packet {
	ether_hdr_t ether_hdr; // set type 0x0806 = ARP
	u16 hw_type; // 00 01 = ethernet
	u16 protocol_type; //0x0800 = IP
	u8 hw_size; // ==6 (mac addr)
	u8 protocol_size; // == 4 (ipv4)
	u16 opcode; // 1 = request, 2 = reply
	u8 sender_mac[6];
	u32 sender_ip;
	u8 target_mac[6];
	u32 target_ip;
} arp_packet_t;

/************************** Function Implemetations *****************************/

void eth_frame_fill_ether_hdr(unsigned char *ef, const unsigned char *src_mac, const unsigned char *dst_mac)
{
	ether_hdr_t *eh = (ether_hdr_t *)ef;

	unsigned char *frame = eh->dst_mac;
	/* mac addresses */
	int i = 6;
	while (i--) *frame++ = *dst_mac++;
	i = 6;
	frame = eh->src_mac;
	while (i--) *frame++ = *src_mac++;

	eh->ethertype = htons(IP_VERSION_4); /* typeip = 0x0800, microblaze is little endian */
}

void eth_frame_fill_ip_hdr(unsigned char *ef, u32 src_ip, u32 dst_ip, u16 total_length)
{
	ip_hdr_t *ip_hdr = &(((ip_packet_t *)ef)->ip_hdr);

	ip_hdr->version_header_length = 0x45; /* IPv4, 5 * 4 = 20 bytes length (no options) */
	ip_hdr->dscp = 0; // ???
	ip_hdr->total_length = htons(total_length); // ?? not correct filled in by hardware
	//ip_hdr->total_length = 0; // ?? not correct filled in by hardware
	ip_hdr->identification = 0; // disabled ?
	//ip_hdr->flags = 0; // could be do not fragment
	ip_hdr->flags = 0x40; // don't fragment flag
	ip_hdr->time_to_live = 128; // random?
	ip_hdr->protocol = IP_HEADER_PROTOCOL_UDP; // 17 = UDP
	ip_hdr->header_checksum = 0; // disabled (filled in by hardware)
	ip_hdr->source_ip = src_ip;
	ip_hdr->destination_ip = dst_ip;
}

void eth_frame_fill_udp_hdr(unsigned char *ef, u16 src_port, u16 dst_port, u16 length)
{
	udp_hdr_t *udp_hdr = &(((ip_udp_packet_t *)ef)->udp_hdr);

	udp_hdr->source_port = htons(src_port);
	udp_hdr->destination_port = htons(dst_port);
	udp_hdr->length = htons(length);
	udp_hdr->checksum = 0; // disabled ?? (filled in by hardware)
}

unsigned char *eth_frame_fill_ether_ip_udp_hdr(unsigned char *ef,
		const unsigned char *src_mac, const unsigned char *dst_mac,
		u32 src_ip, u32 dst_ip,
		u16 src_port, u16 dst_port,
		u16 payload_length)
{

	/* clear the frame */
	//memset(ef, 0, payload_length + sizeof(ip_udp_packet_t));

	eth_frame_fill_ether_hdr(ef, src_mac, dst_mac);
	eth_frame_fill_ip_hdr(ef, src_ip, dst_ip, payload_length + 8 + 20);
	eth_frame_fill_udp_hdr(ef, src_port, dst_port, payload_length + 8); // 8 bytes UDP header.
	unsigned char *data = &(((ip_udp_packet_t *)ef)->data[0]);

	// return pointer to data section of the packet.
	return data;
}


int eth_frame_test_udp_packet(unsigned char *ef, int payload_length)
{

	/* clear the frame */
	memset(ef, 0, payload_length + sizeof(ip_udp_packet_t));

	eth_frame_fill_ether_hdr(ef, TestMAC, BroadcastMAC);
	eth_frame_fill_ip_hdr(ef, QUAD_TO_IP(192,168,10,99), QUAD_TO_IP(192,168,10,1), payload_length+8+20);
	eth_frame_fill_udp_hdr(ef, 2000, 2001, payload_length);

	unsigned char *data = &(((ip_udp_packet_t *)ef)->data[0]);

	int i;
	for (i = 0; i < payload_length; i++) data[i] = (u8)i;

	return (payload_length + sizeof(ip_udp_packet_t));
}


#endif /* ETH_PACKET_UTIL_H_ */
