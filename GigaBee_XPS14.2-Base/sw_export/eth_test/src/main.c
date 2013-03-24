/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 07.01.2013
 */
#include "xparameters.h"
#include <stdio.h>
#include "xil_cache.h"
#include "eth_hw.h"
#include "eth_base.h"
#include "1wire.h"
#include "eth_arp.h"
#include "eth_icmp.h"
#include "eth_udp_tftp.h"
#include "spi_flash.h"
#include "xspi.h"
#include "xuartns550.h"
//=============================================================================
#define FLASH_ERR		0x02
#define OW_ERR			0x04
#define MCB1_ERR		0x08
#define MCB3_ERR		0x10
#define PMOD_ERR		0x20
#define ETH_ERR			0x40
//=============================================================================
// Global Variables
u32* mcb3_ddr = (u32*)XPAR_MCB3_DDR3_S0_AXI_BASEADDR;
u32* mcb1_ddr = (u32*)XPAR_MCB1_DDR3_S0_AXI_BASEADDR;
u8* read_buf = (u8*)(XPAR_MCB3_DDR3_S0_AXI_BASEADDR + 0x01000000);

volatile u32 *pb_and_sw = (u32*)0x81440000;
volatile u32 *leds = (u32*)0x81420000;
volatile u32 *pmod = (u32*)0x81450000;

u8 ow_addr[8]  = {0x89,0,0,0,0,0,0,0};					// DS2502-E48 Family
u8 mac_addr[6] = {0x00, 0x0A, 0x35, 0x01, 0x02, 0x03};	// Default MAC
u8 ip_addr[4]  = {192,168,42,88};						// IP addr
u8 rx_packet[ETH_FRAME_SIZE];

u32 status = 0x0180;	// Status LEDs is off
tftp_session_t tftp_session;
arp_session_t arp_session;
//=============================================================================
int process_udp(u8 *packet, u32 len){
	u16 dst_port;
	dst_port = eth_get_u16(packet, ETH_FRAME_UDP_DST_PORT_OFF);
	if(dst_port == UDP_TFTP_PORT){	// Process TFTP
		return tftp_server(&tftp_session, packet, len);
	}
	return 0;
}
//=============================================================================
void process_file(u8 *buffer, u32 len, u8* file_name){
	u32 start_addr = 0;
	u32 start_offset = 0;
	int i,j;

	xil_printf("Processing file %s \r\n",file_name);
	for(i = 0; i < 200; i++){
		if((buffer[i] == 0xFF) && (buffer[i+1] == 0xFF)){
			start_offset = i;
			xil_printf("Bitstream offset %d bytes\r\n",start_offset);
			for(j=0;j<(len-start_offset);j++){
				buffer[j] = buffer[j+start_offset];
			}
			break;
		}
	}

	xil_printf("Start writing Flash from 0x%08X %d bytes\r\n",start_addr,(len - start_offset));
	spi_flash_write(start_addr, (len - start_offset), buffer);
	spi_flash_read(start_addr, (len - start_offset), read_buf);
	for(i = 0; i<(len - start_offset); i++){
		if(read_buf[i] != buffer[i]){
			xil_printf("Data check failed at 0x%08X\r\n",i);
			for(j = 0; j < 10; j++){
				xil_printf("%02x != %02x\r\n",read_buf[i+j], buffer[i+j]);
			}
			break;
		}
	}
	xil_printf("Done\r\n");
}

void show_error(u32 errno){
	volatile u32 i;
	while(1){
		leds[0]	= ~errno;
		for(i = 0; i < 250000; i++);
		leds[0]	= 0xffffffff;
		for(i = 0; i < 250000; i++);
	}
}
//=============================================================================
int main(void){
	u32 len, slen;
	int i,j;
	u8 h_crc, d_crc;
	int current_link_speed = 1;	// Dummy value to init first message
	int link_speed;
	int ptype;
	u32 flash_id;

    XUartNs550_SetBaud(XPAR_RS232_UART_1_BASEADDR, XPAR_RS232_UART_1_CLOCK_FREQ_HZ, 115200);
    spi_flash_init(XPAR_QSPI_FLASH_DEVICE_ID);

	print("\r\nTE0600 Base project "__DATE__" "__TIME__"\r\n\r\n");
	flash_id = spi_flash_get_id();
    xil_printf("Flash ID 0x%08x",flash_id);
    if(flash_id == 0x00EF4017){
    	xil_printf(" (W25Q64FV)\r\n");
    }
    else if(flash_id == 0x00EF4018){
    	xil_printf(" (W25Q128FV)\r\n");
    }
    else{
    	xil_printf("Failed\r\n");
		show_error(FLASH_ERR);
    }

	if(one_wire_reset(XPAR_AXI_1WIRE_0_BASEADDR)){
		xil_printf("One wire test failed\r\n");
		show_error(OW_ERR);
	}
	else{
		one_wire_search(XPAR_AXI_1WIRE_0_BASEADDR, ow_addr, 8);
		xil_printf("OW  addr %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x \r\n",
				ow_addr[0],ow_addr[1],ow_addr[2],ow_addr[3],ow_addr[4],
				ow_addr[5],ow_addr[6],ow_addr[7]);
		one_wire_reset(XPAR_AXI_1WIRE_0_BASEADDR);
		one_wire_write_byte(XPAR_AXI_1WIRE_0_BASEADDR, ONE_WIRE_CMD_MATCH_ROM);
		for(i = 0; i < 8; i++)
			one_wire_write_byte(XPAR_AXI_1WIRE_0_BASEADDR, ow_addr[i]);
		one_wire_write_byte(XPAR_AXI_1WIRE_0_BASEADDR, DS2502_CMD_READ_MEMORY);
		h_crc = one_wire_crc8(0, DS2502_CMD_READ_MEMORY);
		one_wire_write_byte(XPAR_AXI_1WIRE_0_BASEADDR, 0);
		h_crc = one_wire_crc8(h_crc, 0);
		one_wire_write_byte(XPAR_AXI_1WIRE_0_BASEADDR, 0);
		h_crc = one_wire_crc8(h_crc, 0);
		d_crc = one_wire_read_byte(XPAR_AXI_1WIRE_0_BASEADDR);
		if(h_crc == d_crc)
			for(j = 0; j < 13; j++){
				i = one_wire_read_byte(XPAR_AXI_1WIRE_0_BASEADDR);
				if((j >= 5) && (j <= 10))
					mac_addr[j-5] = i;
		}
	}
	xil_printf("MAC addr %02x:%02x:%02x:%02x:%02x:%02x \r\n",
		mac_addr[0],mac_addr[1],mac_addr[2],
		mac_addr[3],mac_addr[4],mac_addr[5]);

	xil_printf("IP  addr %d.%d.%d.%d \r\n", ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);


	// DDR memory testing
	for(i = 0; i < 100000; i++){
		mcb1_ddr[i] = i;
		mcb3_ddr[i] = i;
	}
	xil_printf("MCB1 DDR Memory test - ");
	for(i = 0; i < 100000; i++){
		if(mcb1_ddr[i] != i){
			xil_printf("Failed\r\n");
			show_error(MCB1_ERR);
			break;
		}
	}
	xil_printf("Passed\r\n");
	xil_printf("MCB3 DDR Memory test - ");
	for(i = 0; i < 100000; i++){
		if(mcb3_ddr[i] != i){
			xil_printf("Failed\r\n");
			show_error(MCB3_ERR);
			break;
		}
	}
	xil_printf("Passed\r\n");

	// Pmod connectors testing
	xil_printf("Pmod test            - ");
	pmod[1] = 0xffffffff;	// Port 1 as input
	pmod[2]	= 0;
	// Test for 0
	if(pmod[0] != 0){
		xil_printf("Failed\r\n");
		//show_error(PMOD_ERR);
	}
	else{
		for(i = 0; i < 24; i++){	// Test for 1
			pmod[2] = (1 << i);
			j = pmod[0] & (1 << i);
			if(!j){
				xil_printf("Failed\r\n");
				//show_error(PMOD_ERR);
				break;
			}
		}
	}
	if(j == 24){
		xil_printf("Passed\r\n");
	}


	// Initialize network services
	eth_arp_init(&arp_session, ip_addr, mac_addr);
	tftp_init(&tftp_session, (u8*)(XPAR_MCB3_DDR3_S0_AXI_BASEADDR + 0x01000000), (void*)&process_file);

#if XPAR_MICROBLAZE_USE_ICACHE
	Xil_ICacheInvalidate();
	Xil_ICacheEnable();
#endif
#if XPAR_MICROBLAZE_USE_DCACHE
	Xil_DCacheInvalidate();
	Xil_DCacheEnable();
#endif

	xil_printf("Initializing Ethernet  ");
	if(eth_init(XPAR_AXIETHERNET_0_DEVICE_ID, mac_addr, ip_addr) != XST_SUCCESS){
		xil_printf("Failed\r\n");
		show_error(ETH_ERR);
	}
	else
		xil_printf("Done\r\n");


	while(1){
		link_speed = eth_get_link_speed();
		if(link_speed != current_link_speed){
			if(link_speed != 0){
				eth_update_link_speed(link_speed);
				xil_printf("Link is UP %d MBit\r\n",link_speed);
			}
			else
				xil_printf("Link is Down\r\n",link_speed);
			current_link_speed = link_speed;
		}

		if(current_link_speed){	// Receive only if link is UP
			len = eth_receive_packet(rx_packet);
			if (len != 0){		// We receive something
				//xil_printf("*");
				ptype = eth_get_packet_type(rx_packet, len);
				switch (ptype) {
					case ETH_PACKET_TYPE_UNKNOWN:
						xil_printf("Unknown\r\n");
						break;
					case ETH_PACKET_TYPE_ARP:
						if(eth_check_arp_dst(&arp_session, rx_packet)){
							slen = eth_process_arp(&arp_session, rx_packet);
							eth_send_packet(rx_packet, len);
						}
						break;
					case ETH_PACKET_TYPE_ICMP:
						if(eth_check_ipv4_dst(rx_packet)){
							slen = eth_process_icmp(rx_packet, len);
							eth_send_packet(rx_packet, slen);
						}
						break;
					case ETH_PACKET_TYPE_UDP:
						if(eth_check_ipv4_dst(rx_packet)){
							slen = process_udp(rx_packet,len);
							eth_send_packet(rx_packet, slen);
						}
						break;
					default:
						break;
				}
			} 		// Receive
			else{	// Nothing received - Housekeeping
				// Read switches and buttons and set LEDs
				leds[0] = (pb_and_sw[0] | status);

				slen = tftp_server(&tftp_session, rx_packet, 0);	// Run TFTP
				eth_send_packet(rx_packet, slen);
			}
		} // current_link_speed
	} // while(1)

	return 0;
}
