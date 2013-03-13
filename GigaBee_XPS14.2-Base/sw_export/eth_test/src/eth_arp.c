/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 18.01.2013
 *
 */
#include "eth_arp.h"

int eth_arp_init(arp_session_t* session, u8* ip_ptr, u8* mac_ptr){
	int i;

	for(i = 0; i < 4; i++){
		session->ip[i] = ip_ptr[i];
	}
	for(i = 0; i < 6; i++){
		session->mac[i] = mac_ptr[i];
	}

	return 0;
}

int eth_check_arp_dst(arp_session_t* session, u8 *packet){
	if(((packet[ETH_FRAME_ARP_TARGET_IP_OFF + 0] == session->ip[0]) &&
		(packet[ETH_FRAME_ARP_TARGET_IP_OFF + 1] == session->ip[1]) &&
		(packet[ETH_FRAME_ARP_TARGET_IP_OFF + 2] == session->ip[2]) &&
		(packet[ETH_FRAME_ARP_TARGET_IP_OFF + 3] == session->ip[3])))
		return 1;
	else
		return 0;
}

int eth_process_arp(arp_session_t* session, u8* packet){
	int i;
	for(i = 0; i < 6; i++)	// Fill DST MAC
		packet[ETH_FRAME_DST_MAC_OFF + i] = packet[ETH_FRAME_SRC_MAC_OFF + i];
	for(i = 0; i < 6; i++)	// Fill SRC MAC
		packet[ETH_FRAME_SRC_MAC_OFF + i] = session->mac[i];
	packet[ETH_FRAME_ARP_OPERATION_OFF] = ETH_ARP_OPERATION_REPLY;
	for(i = 0; i < 10; i++)	// Copy sender MAC and IP to target field
		packet[ETH_FRAME_ARP_TARGET_MAC_OFF + i] = packet[ETH_FRAME_ARP_SENDER_MAC_OFF + i];
	for(i = 0; i < 6; i++)	// Fill DST MAC
		packet[ETH_FRAME_ARP_SENDER_MAC_OFF + i] = session->mac[i];
	for(i = 0; i < 4; i++)	// Fill DST IP
		packet[ETH_FRAME_ARP_SENDER_IP_OFF + i] = session->ip[i];
	return 42;
}



