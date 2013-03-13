/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 17.01.2013
 *
 */

#ifndef ETH_UDP_TFTP_H_
#define ETH_UDP_TFTP_H_

#include "eth_udp.h"

#define	UDP_TFTP_OPCODE_OFF		ETH_FRAME_IPV4_DATA_OFF + UDP_HEADER_SIZE + 1
#define UDP_TFTP_BLOCKNUM_OFF	ETH_FRAME_IPV4_DATA_OFF + UDP_HEADER_SIZE + 2
#define	UDP_TFTP_FILENAME_OFF	ETH_FRAME_IPV4_DATA_OFF + UDP_HEADER_SIZE + 2
#define	UDP_TFTP_DATA_OFF		ETH_FRAME_IPV4_DATA_OFF + UDP_HEADER_SIZE + 4
#define UDP_TFTP_HEADER_SIZE	4

#define TFTP_RRQ				1
#define TFTP_WRQ				2
#define TFTP_DATA				3
#define TFTP_ACK				4
#define TFTP_ERROR				5

#define TFTP_ACK_SIZE			4
#define TFTP_ERR_SIZE			4

// States
#define TFTP_IDLE				0
#define TFTP_RECEIVING			1
#define TFTP_PROCESSING			2
#define TFTP_COMPLETE			3
#define TFTP_SENDING			4

// Errors
#define TFTP_ERR_NO_FILE			1
#define TFTP_ERR_UNEXPECTED_DATA	5

typedef struct tftp_session {
	u8 state;
	u8* file_buffer;
	u32 file_pointer;
	u8 file_name[20];
	u32 file_size;
	void (*file_processing_func)(u8*,u32,u8*);
} tftp_session_t;

// Function prototypes
int tftp_init(tftp_session_t* session, u8* buf_ptr, void* func);
int tftp_make_ack(tftp_session_t* session, u8 *packet, u32 len, u16 block_num);
int tftp_make_error(tftp_session_t* session, u8 *packet, u32 len, u8 errno);
int tftp_add_data(tftp_session_t* session, u8 *packet);
int tftp_server(tftp_session_t* session, u8 *packet, u32 len);

#endif /* ETH_UDP_TFTP_H_ */
