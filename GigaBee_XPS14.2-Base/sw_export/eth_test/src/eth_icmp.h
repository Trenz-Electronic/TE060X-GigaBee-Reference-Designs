/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 17.01.2013
 *
 */
#ifndef ETH_ICMP_H_
#define ETH_ICMP_H_

#include "xbasic_types.h"
#include "eth_base.h"

// ICMP Packet Offsets
#define	ETH_FRAME_ICMP_TYPE_OFF				34
#define	ETH_FRAME_ICMP_CODE_OFF				35
#define	ETH_FRAME_ICMP_CS_OFF				36
#define	ETH_FRAME_ICMP_DATA_OFF				38
// ICMP Packet Definitions
#define	ETH_FRAME_ICMP_TYPE_ECHO_REQ		8
#define	ETH_FRAME_ICMP_TYPE_ECHO_REPLY		0


// Functions prototypes
void eth_set_icmp_sc(u8* packet, u32 len);
int eth_process_icmp(u8* packet, u32 len);

#endif
