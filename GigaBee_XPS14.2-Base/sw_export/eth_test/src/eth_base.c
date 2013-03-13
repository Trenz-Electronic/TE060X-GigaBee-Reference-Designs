/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 07.01.2013
 *
 */
#include "eth_base.h"
#include "eth_hw.h"
#include "stdio.h"


void eth_swap_macs(u8* packet){
	u8 tmp[6];
	int i;
	for(i = 0; i < 6; i++)
		tmp[i] = packet[ETH_FRAME_DST_MAC_OFF + i];
	for(i = 0; i < 6; i++)
		packet[ETH_FRAME_DST_MAC_OFF + i] = packet[ETH_FRAME_SRC_MAC_OFF + i];
	for(i = 0; i < 6; i++)
		packet[ETH_FRAME_SRC_MAC_OFF + i] = tmp[i];
}

void eth_swap_ips(u8* packet){
	u8 tmp[4];
	int i;
	for(i = 0; i < 4; i++)
		tmp[i] = packet[ETH_FRAME_IPV4_DST_IP_OFF + i];
	for(i = 0; i < 4; i++)
		packet[ETH_FRAME_IPV4_DST_IP_OFF + i] = packet[ETH_FRAME_IPV4_SRC_IP_OFF + i];
	for(i = 0; i < 4; i++)
		packet[ETH_FRAME_IPV4_SRC_IP_OFF + i] = tmp[i];
}

u16 eth_calc_cs(u8* packet, u32 len){
	u32 checksum;
	u16 w16;
	int loop;

	checksum = 0;
	w16 = 0;

	for (loop = 0; loop < len; loop += 2) {
		w16 = ((packet[loop] << 8) & 0xFF00) + (packet[loop + 1] & 0x00FF);
		checksum += (u32) w16;
	}
	while (checksum >> 16) {
		checksum = (checksum & 0xFFFF) + (checksum >> 16);
	}
	return (u16) (~checksum);
}

void eth_set_ipv4_sc(u8* packet){
	u16 cs;
	eth_set_u16(packet, ETH_FRAME_IPV4_CS_OFF, 0);
	cs = eth_calc_cs((packet + ETH_FRAME_DATA_OFF), ETH_FRAME_IPV4_HEADER_SIZE);
	eth_set_u16(packet, ETH_FRAME_IPV4_CS_OFF, cs);
}

int eth_get_packet_type(u8 *packet, u32 len){
	if(packet[ETH_FRAME_ETHERTYPE_OFF] != 0x08) return ETH_PACKET_TYPE_UNKNOWN;
	if(packet[ETH_FRAME_ETHERTYPE_OFF + 1] == 0x06) return ETH_PACKET_TYPE_ARP;
	if(packet[ETH_FRAME_ETHERTYPE_OFF + 1] == 0x00){	// IPv4
		switch(packet[ETH_FRAME_IPV4_PROTOCOL_OFF]){
			case ETH_FRAME_IPV4_PROTOCOL_ICMP:
				return ETH_PACKET_TYPE_ICMP;
				break;
			case ETH_FRAME_IPV4_PROTOCOL_UDP:
				return ETH_PACKET_TYPE_UDP;
				break;
			case ETH_FRAME_IPV4_PROTOCOL_TCP:
				return ETH_PACKET_TYPE_TCP;
				break;
			default:
				return ETH_PACKET_TYPE_UNKNOWN;
				break;
		}
	}
	else
		return ETH_PACKET_TYPE_UNKNOWN;
}

int eth_check_ipv4_dst(u8 *packet){
	if(((packet[ETH_FRAME_IPV4_DST_IP_OFF + 0] == eth_ip_addr[0]) &&
		(packet[ETH_FRAME_IPV4_DST_IP_OFF + 1] == eth_ip_addr[1]) &&
		(packet[ETH_FRAME_IPV4_DST_IP_OFF + 2] == eth_ip_addr[2]) &&
		(packet[ETH_FRAME_IPV4_DST_IP_OFF + 3] == eth_ip_addr[3])))
		return 1;
	else
		return 0;
}

u32 eth_get_u32(u8 *packet, u32 off){
	return (packet[off + 0] << 24) + (packet[off + 1] << 16) + (packet[off + 2] << 8) + packet[off + 3];
}

void eth_set_u32(u8 *packet, u32 off, u32 val){
	packet[off + 0] = (u8)((val & 0xFF000000) >> 24);
	packet[off + 1] = (u8)((val & 0x00FF0000) >> 16);
	packet[off + 2] = (u8)((val & 0x0000FF00) >>  8);
	packet[off + 3] = (u8)(val & 0x000000FF);
}

u16 eth_get_u16(u8 *packet, u32 off){
	return (packet[off + 0] << 8) + packet[off + 1];
}

void eth_set_u16(u8 *packet, u32 off, u16 val){
	packet[off + 0] = (u8)((val & 0xFF00) >> 8);
	packet[off + 1] = (u8)(val & 0x00FF);
}
