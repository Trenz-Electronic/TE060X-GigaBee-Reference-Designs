/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 07.01.2013
 *
 */

#ifndef ETH_FIFO_BASE_H_
#define ETH_FIFO_BASE_H_

#include "xbasic_types.h"

#define ETH_FRAME_SIZE 2000

// Ethernet Frame Offsets
#define	ETH_FRAME_DST_MAC_OFF				0
#define	ETH_FRAME_SRC_MAC_OFF				6
#define	ETH_FRAME_ETHERTYPE_OFF				12
#define ETH_FRAME_HEADER_SIZE				14
#define ETH_FRAME_DATA_OFF	 				14
// Ethernet Frame Definitions
#define	ETH_FRAME_ETHERTYPE_IPV4			0x0800
#define	ETH_FRAME_ETHERTYPE_ARP				0x0806

// IPv4 Packet Offsets
#define	ETH_FRAME_IPV4_TOTAL_LENGTH_OFF		16
#define	ETH_FRAME_IPV4_ID_OFF				18
#define	ETH_FRAME_IPV4_FLAGS_OFF			20
#define	ETH_FRAME_IPV4_TTL_OFF				22
#define	ETH_FRAME_IPV4_PROTOCOL_OFF			23
#define	ETH_FRAME_IPV4_CS_OFF				24
#define	ETH_FRAME_IPV4_SRC_IP_OFF			26
#define	ETH_FRAME_IPV4_DST_IP_OFF			30
#define ETH_FRAME_IPV4_DATA_OFF 			34
// IPv4 Packet Definitions
#define ETH_FRAME_IPV4_HEADER_SIZE			20
#define	ETH_FRAME_IPV4_PROTOCOL_ICMP		0x01
#define	ETH_FRAME_IPV4_PROTOCOL_TCP			0x06
#define	ETH_FRAME_IPV4_PROTOCOL_UDP			0x11

// Type definitions return statuses
#define ETH_PACKET_TYPE_UNKNOWN				0
#define	ETH_PACKET_TYPE_ARP					1
#define	ETH_PACKET_TYPE_ICMP				2
#define	ETH_PACKET_TYPE_UDP					3
#define	ETH_PACKET_TYPE_TCP					4

// Function Prototypes
int eth_get_packet_type(u8 *packet, u32 len);
int eth_check_ipv4_dst(u8 *packet);
void eth_swap_macs(u8* packet);
void eth_swap_ips(u8* packet);
void eth_set_ipv4_sc(u8* packet);
void eth_set_icmp_sc(u8* packet, u32 len);
u16 eth_calc_cs(u8* packet, u32 len);
u32 eth_get_u32(u8 *packet, u32 off);
u16 eth_get_u16(u8 *packet, u32 off);
void eth_set_u32(u8 *packet, u32 off, u32 val);
void eth_set_u16(u8 *packet, u32 off, u16 val);



#endif /* ETH_FIFO_BASE_H_ */
