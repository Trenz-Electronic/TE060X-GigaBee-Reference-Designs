/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 18.01.2013
 *
 */
#include "eth_udp.h"
//==============================================================================
int udp_swap_ports(u8* packet){
	u8 ph, pl;

	ph = packet[ETH_FRAME_UDP_SRC_PORT_OFF + 0];
	pl = packet[ETH_FRAME_UDP_SRC_PORT_OFF + 1];
	packet[ETH_FRAME_UDP_SRC_PORT_OFF + 0] = packet[ETH_FRAME_UDP_DST_PORT_OFF + 0];
	packet[ETH_FRAME_UDP_SRC_PORT_OFF + 1] = packet[ETH_FRAME_UDP_DST_PORT_OFF + 1];
	packet[ETH_FRAME_UDP_DST_PORT_OFF + 0] = ph;
	packet[ETH_FRAME_UDP_DST_PORT_OFF + 1] = pl;
	return 0;
}
