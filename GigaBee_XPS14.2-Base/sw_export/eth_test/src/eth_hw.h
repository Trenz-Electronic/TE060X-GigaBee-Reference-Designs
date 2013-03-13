/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 07.01.2013
 *
 */

#ifndef ETH_HW_H_
#define ETH_HW_H_

// Includes
#include "xbasic_types.h"
#include "xaxiethernet.h"
#include "xllfifo.h"

// Macros for LocalLink FIFO
#define LL_FIFO_RX_DATA		0
#define LL_FIFO_RX_FLAGS	1
#define LL_FIFO_RX_OCC		2
#define LL_FIFO_RX_RST		3
#define LL_FIFO_RX_EMPTY	0x00000001
#define LL_FIFO_RX_FULL		0x00000002
#define LL_FIFO_RX_SFLAG	0x00000004
#define LL_FIFO_RX_EFLAG	0x00000008
#define LL_FIFO_RX_BEFLAG	0x00000030
#define LL_FIFO_BE1			0x00000010
#define LL_FIFO_BE2			0x00000020
#define LL_FIFO_BE3			0x00000030
#define LL_FIFO_BE4			0x00000000
#define LL_RESET_VECTOR		0x000000a5

int eth_init(u32 eth_id, u8* mac, u8* ip);
int eth_receive_packet(u8 *packet);
int eth_send_packet(u8 *packet, u32 len);
u16 eth_get_link_speed(void);
void eth_update_link_speed(u16 speed);
int eth_stop(void);

// Variable Definitions
u8 eth_mac_addr[6];
u8 eth_ip_addr[4];

XAxiEthernet AxiEthernetInstance; 	// axi_ethernet
XLlFifo FifoInstance;

#endif /* ETH_HW_H_ */
