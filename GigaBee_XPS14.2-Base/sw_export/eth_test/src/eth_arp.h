/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 17.01.2013
 *
 */

#ifndef ETH_ARP_H_
#define ETH_ARP_H_

#include "xbasic_types.h"
#include "eth_base.h"

// ARP Packet Offsets
#define	ETH_FRAME_ARP_OPERATION_OFF			21
#define	ETH_FRAME_ARP_SENDER_MAC_OFF		22
#define	ETH_FRAME_ARP_SENDER_IP_OFF			28
#define	ETH_FRAME_ARP_TARGET_MAC_OFF		32
#define	ETH_FRAME_ARP_TARGET_IP_OFF			38
// ARP Packet definitions
#define	ETH_ARP_OPERATION_REQUEST			1
#define	ETH_ARP_OPERATION_REPLY				2

typedef struct arp_session {
	u8 ip[4];
	u8 mac[6];
} arp_session_t;

// Function definitions
int eth_arp_init(arp_session_t* session, u8* ip_ptr, u8* mac_ptr);
int eth_check_arp_dst(arp_session_t* session, u8 *packet);
int eth_process_arp(arp_session_t* session, u8* packet);

#endif /* ETH_ARP_H_ */
