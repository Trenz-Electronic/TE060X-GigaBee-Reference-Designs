/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 18.01.2013
 *
 */

#ifndef ETH_UDP_H_
#define ETH_UDP_H_

#include "eth_base.h"

#define UDP_TFTP_PORT						69

#define	ETH_FRAME_UDP_SRC_PORT_OFF			ETH_FRAME_IPV4_DATA_OFF + 0
#define	ETH_FRAME_UDP_DST_PORT_OFF			ETH_FRAME_IPV4_DATA_OFF + 2
#define	ETH_FRAME_UDP_LENGTH_OFF			ETH_FRAME_IPV4_DATA_OFF + 4
#define	ETH_FRAME_UDP_CS_OFF				ETH_FRAME_IPV4_DATA_OFF + 6
#define	ETH_FRAME_UDP_DATA_OFF				ETH_FRAME_IPV4_DATA_OFF + 8
#define UDP_HEADER_SIZE						8

int udp_swap_ports(u8* packet);

#endif /* ETH_UDP_H_ */
