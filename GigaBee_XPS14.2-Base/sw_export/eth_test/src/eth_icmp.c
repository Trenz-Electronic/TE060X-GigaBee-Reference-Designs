/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 18.01.2013
 *
 */
#include "eth_icmp.h"

void eth_set_icmp_sc(u8* packet, u32 len){
	u16 cs;
	packet[ETH_FRAME_ICMP_CS_OFF + 0] = 0;
	packet[ETH_FRAME_ICMP_CS_OFF + 1] = 0;
	cs = eth_calc_cs((packet + ETH_FRAME_IPV4_DATA_OFF), len - ETH_FRAME_IPV4_DATA_OFF);
	packet[ETH_FRAME_ICMP_CS_OFF + 0] = cs >> 8;
	packet[ETH_FRAME_ICMP_CS_OFF + 1] = cs & 0xFF;
}

int eth_process_icmp(u8* packet, u32 len){
	eth_swap_macs(packet);
	if(packet[ETH_FRAME_IPV4_ID_OFF + 1] < 0xFF)
		packet[ETH_FRAME_IPV4_ID_OFF + 1]++;
	else{
		packet[ETH_FRAME_IPV4_ID_OFF + 0]++;
		packet[ETH_FRAME_IPV4_ID_OFF + 1] = 0;
	}
	eth_swap_ips(packet);
	eth_set_ipv4_sc(packet);
	packet[ETH_FRAME_ICMP_TYPE_OFF] = ETH_FRAME_ICMP_TYPE_ECHO_REPLY;
	eth_set_icmp_sc(packet, len);
	return len;
}

