/*
 * Copyright (c) 2007 Xilinx, Inc.  All rights reserved.
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

#include "lwip/err.h"
#include "lwip/udp.h"

static unsigned rxperf_port = 5001;
static unsigned rxperf_server_running = 0;

int
transfer_urxperf_data() {
	return 0;
}

void
urxperf_recv_callback(void *arg, struct udp_pcb *tpcb,
                               struct pbuf *p, struct ip_addr *addr, u16_t port)
{
	static int first = 1;
	static int expected_id = 0;
	static int n_dropped = 0;
	int recv_id;

	/* first, see if the datagram is received in order */
	recv_id = ((*(u16_t*)(p->payload)) << 16) | (*(u16_t*)(p->payload + 2) & 0xffff);

	if (first)
		expected_id = recv_id;
	else if (expected_id != recv_id)  {
		n_dropped += (recv_id - expected_id);

		if (n_dropped > 100) {
			/* print error for every 1000 dropped packets */
			xil_printf("e: %d, r: %d\r\n", expected_id, recv_id);
			n_dropped = 0;
		}

		expected_id = recv_id;
	}

	expected_id++;
	pbuf_free(p);

	first = 0;
}

int
start_urxperf_application()
{
	struct udp_pcb *pcb;
	err_t err;
	unsigned port = rxperf_port;	/* iperf default port */

	/* create new TCP PCB structure */
	pcb = udp_new();
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\r\n");
		return -1;
	}

	/* bind to iperf @port */
	err = udp_bind(pcb, IP_ADDR_ANY, port);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\r\n", port, err);
		return -2;
	}

	udp_recv(pcb, urxperf_recv_callback, NULL);

        rxperf_server_running = 1;

	return 0;
}

void print_urxperf_app_header()
{
    xil_printf("%20s %6d %10s %s\r\n", "rxperf server",
                        rxperf_port,
                        rxperf_server_running ? "RUNNING" : "INACTIVE",
                        "$ iperf -c <board ip> -i 5 -t 100 -u -b <bandwidth>");
}
