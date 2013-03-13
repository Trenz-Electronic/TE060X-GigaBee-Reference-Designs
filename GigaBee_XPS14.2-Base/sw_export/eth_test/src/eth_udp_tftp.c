/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 17.01.2013
 *
 */

#include "eth_udp_tftp.h"
#include <stdio.h>

//==============================================================================
int tftp_init(tftp_session_t* session, u8* buf_ptr, void* func){
	session->state = TFTP_IDLE;
	session->file_pointer = 0;
	session->file_buffer = buf_ptr;
	session->file_processing_func = func;
	return 0;
}
//==============================================================================
int tftp_make_ack(tftp_session_t* session, u8 *packet, u32 len, u16 block_num){
	// Payload
	eth_set_u16(packet, (ETH_FRAME_UDP_DATA_OFF + 0), TFTP_ACK);
	eth_set_u16(packet, (ETH_FRAME_UDP_DATA_OFF + 2), block_num);
	// Ethernet
	eth_swap_macs(packet);
	// IPv4
	eth_swap_ips(packet);
	eth_set_u16(packet, ETH_FRAME_IPV4_TOTAL_LENGTH_OFF,
			(ETH_FRAME_IPV4_HEADER_SIZE + UDP_HEADER_SIZE + TFTP_ACK_SIZE));
	eth_set_ipv4_sc(packet);
	// UDP
	udp_swap_ports(packet);
	eth_set_u16(packet, ETH_FRAME_UDP_LENGTH_OFF,
			(UDP_HEADER_SIZE + TFTP_ACK_SIZE));
	eth_set_u16(packet, ETH_FRAME_UDP_CS_OFF, 0);
	return (ETH_FRAME_HEADER_SIZE +
			ETH_FRAME_IPV4_HEADER_SIZE +
			UDP_HEADER_SIZE + TFTP_ACK_SIZE);
}
//==============================================================================
int tftp_make_error(tftp_session_t* session, u8 *packet, u32 len, u8 errno){
	char tftp_err1_str[] = "File not found";
	char tftp_err5_str[] = "Unknown transfer ID";
	int i = 0;
	u16 packet_size;

	eth_set_u16(packet, (ETH_FRAME_UDP_DATA_OFF + 0), TFTP_ERROR); // Opcode
	eth_set_u16(packet, (ETH_FRAME_UDP_DATA_OFF + 2), errno); // ErrorCode

	switch(errno){
		case TFTP_ERR_NO_FILE:
			do{
				packet[ETH_FRAME_UDP_DATA_OFF + 4 + i] = tftp_err1_str[i];
				//i++;
			}
			while(tftp_err1_str[i++]);
			break;
		case TFTP_ERR_UNEXPECTED_DATA:
			do{
				packet[ETH_FRAME_UDP_DATA_OFF + 4 + i] = tftp_err5_str[i];
				//i++;
			}
			while(tftp_err1_str[i++]);
			break;
	}
	packet_size = i;
	xil_printf("TFTP Error - %s\r\n",
			(packet + ETH_FRAME_UDP_DATA_OFF + TFTP_ERR_SIZE));
	// Ethernet
	eth_swap_macs(packet);
	// IPv4
	eth_swap_ips(packet);
	eth_set_u16(packet, ETH_FRAME_IPV4_TOTAL_LENGTH_OFF,
			(ETH_FRAME_IPV4_HEADER_SIZE + UDP_HEADER_SIZE +
					TFTP_ERR_SIZE + packet_size));
	eth_set_ipv4_sc(packet);
	// UDP
	udp_swap_ports(packet);
	eth_set_u16(packet, ETH_FRAME_UDP_LENGTH_OFF,
			(UDP_HEADER_SIZE + TFTP_ERR_SIZE + packet_size));
	eth_set_u16(packet, ETH_FRAME_UDP_CS_OFF, 0);
	return (ETH_FRAME_HEADER_SIZE + ETH_FRAME_IPV4_HEADER_SIZE +
			UDP_HEADER_SIZE + TFTP_ERR_SIZE + packet_size);
}
//=============================================================================
int tftp_add_data(tftp_session_t* session, u8 *packet){
	u16 block_size, i;

	block_size = eth_get_u16(packet,
			ETH_FRAME_UDP_LENGTH_OFF) - UDP_HEADER_SIZE - UDP_TFTP_HEADER_SIZE;
	for(i = 0; i < block_size; i++){
		session->file_buffer[session->file_pointer + i] =
				packet[UDP_TFTP_DATA_OFF + i];
	}
	if(block_size < 512){
		session->state = TFTP_PROCESSING;
		session->file_size = session->file_pointer + block_size;
		session->file_pointer = 0;
	}
	else{
		session->file_pointer += i;
	}

	return 0;
}
//==============================================================================
int tftp_server(tftp_session_t* session, u8 *packet, u32 len){
	u8 opcode;
	u16 block_num, block_size;
	int i = 0;
	if(len != 0){
		opcode = packet[UDP_TFTP_OPCODE_OFF];
		block_num = eth_get_u16(packet,UDP_TFTP_BLOCKNUM_OFF);
		switch(session->state){
			//------------------------------------------------------------------
			case TFTP_IDLE:
				switch(opcode){
					case TFTP_RRQ:
						return tftp_make_error(session, packet, len, TFTP_ERR_NO_FILE);
						break;
					case TFTP_WRQ:
						while(packet[UDP_TFTP_FILENAME_OFF + i]){
							session->file_name[i] = packet[UDP_TFTP_FILENAME_OFF + i];
							i++;
						}
						session->file_name[i] = 0;
						xil_printf("TFTP - Start write to file %s\r\n",session->file_name);
						session->state = TFTP_RECEIVING;
						return tftp_make_ack(session, packet, len, 0);
						break;
					case TFTP_DATA:
						return tftp_make_error(session, packet, len, TFTP_ERR_UNEXPECTED_DATA);
						break;
					case TFTP_ACK:
						break;
					case TFTP_ERROR:
						break;
				}
				return 0;
				break;
			//------------------------------------------------------------------
			case TFTP_RECEIVING:
				//xil_printf("Receiving opcode %d\r\n",opcode);
				switch(opcode){
					case TFTP_RRQ:
						return tftp_make_error(session, packet, len, TFTP_ERR_NO_FILE);
						break;
					case TFTP_WRQ:
						return tftp_make_error(session, packet, len, TFTP_ERR_UNEXPECTED_DATA);
						break;
					case TFTP_DATA:
						tftp_add_data(session, packet);
						block_size = eth_get_u16(packet, ETH_FRAME_UDP_LENGTH_OFF);
						if(block_size < 524){	// 512 bytes max data + UDP and TFTP headers
							xil_printf("TFTP - End of file\r\n");
						}
						return tftp_make_ack(session, packet, len, block_num);
						break;
					case TFTP_ACK:
						return tftp_make_error(session, packet, len, TFTP_ERR_UNEXPECTED_DATA);
						break;
					case TFTP_ERROR:
						// Finish session
						session->state = TFTP_IDLE;
						session->file_pointer = 0;
						return tftp_make_ack(session, packet, len, block_num);
						break;
				}
				return 0;
				break;
			//------------------------------------------------------------------
		}
		return 0;
	}
	else{
		// Do housekeeping
		if(session->state == TFTP_PROCESSING){
			session->file_processing_func(session->file_buffer, session->file_size, session->file_name);
			//session->state = TFTP_COMPLETE;
			session->state = TFTP_IDLE;	// We only receiving files
		}
		return 0;
	}
}
//==============================================================================
