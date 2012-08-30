/*
 * Copyright (c) 2008 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <stdio.h>
#include <string.h>

#include "lwip/inet.h"
#include "lwip/sockets.h"

void print_utx_app_header()
{
	xil_printf("\r\n\r\n-----lwIP Transmit Test [Board -> Host] ------\r\n");
	xil_printf("To perform TCP TX bandwidth calculation, run iperf from your host machine as: \r\n");
	xil_printf("iperf -s -i 5 -t 100\r\n");
	xil_printf("Board should have IP 192.168.1.100 (can be changed in txperf.c)\r\n");
}

#define SEND_BUFSIZE (1440)
static char send_buf[SEND_BUFSIZE];

int utx_application_thread()
{
	struct tcp_pcb *pcb;
	struct ip_addr ipaddr;
	int i, sock;
	struct sockaddr_in serv_addr;

	IP4_ADDR(&ipaddr,  192, 168,   1, 100);		/* iperf server address */
	IP4_ADDR(&ipaddr,  172, 16,   0, 100);		/* iperf server address */

	/* initialize data buffer being sent */
	for (i = 0; i < SEND_BUFSIZE; i++)
		send_buf[i] = (i % 10) + '0';

	if ((sock = lwip_socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		xil_printf("error creating socket\r\n");
		return;
	}

	memset((void*)&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = 5001;
	serv_addr.sin_addr.s_addr = ipaddr.addr;

	print("Connecting to iperf server...");
	if (lwip_connect(sock, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0) {
		xil_printf("error in connect\r\n");
		return;
	}
	print("Connected\r\n");

	while (lwip_write(sock, send_buf, SEND_BUFSIZE) >= 0) {
            int id = 0;
            int *p = (int *)(&send_buf);

            *p = id++;
	}

	print("TX perf stopped\r\n");

	return 0;
}
