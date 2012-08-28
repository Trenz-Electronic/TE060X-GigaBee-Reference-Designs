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

static unsigned utxperf_port = 5001;
static unsigned txperf_client_connected = 0;

static struct udp_pcb *connected_pcb = NULL;
static struct pbuf *pbuf_to_be_sent = NULL;

#define SEND_BUFSIZE (1424)
static char send_buf[SEND_BUFSIZE];

int
transfer_utxperf_data()
{
	int copy = 1;
	err_t err;
	struct udp_pcb *pcb = connected_pcb;
	static int id;
	int *payload;

	if (!connected_pcb) {
                xil_printf("!connected_pcb\r\n");
		return -1;
	}

#define ALLOC_PBUF_ON_TRANSFER 0
#if ALLOC_PBUF_ON_TRANSFER
	pbuf_to_be_sent = pbuf_alloc(PBUF_RAW, 1400, PBUF_POOL);
	if (!pbuf_to_be_sent)
		//xil_printf("error allocating pbuf to send\r\n");
		return -1;
	else {
		//memcpy(pbuf_to_be_sent->payload, send_buf, 1024);
		pbuf_to_be_sent->len = 1400;
	}
#endif

	if (!pbuf_to_be_sent) {
		return -1;
	}

	/* always increment the id */
	payload = (int*)(pbuf_to_be_sent->payload);
	*payload = id++;

	err = udp_send(pcb, pbuf_to_be_sent);
	if (err != ERR_OK) {
		xil_printf("Error on udp_send: %d\r\n", err);
		return -1;
	}

#if ALLOC_PBUF_ON_TRANSFER
	pbuf_free(pbuf_to_be_sent);
#endif

	return 0;
}

int
start_utxperf_application()
{
	struct udp_pcb *pcb;
	struct ip_addr ipaddr;
	err_t err;
	u16_t port;
	int i;

	/* create a udp socket */
	pcb = udp_new();
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\r\n");
		return -1;
	}

	/* bind local address */
	if ((err = udp_bind(pcb, IP_ADDR_ANY, 0)) != ERR_OK) {
		xil_printf("error on udp_bind: %x\n\r", err);
	}

	/* connect to iperf server */
#if 1
	IP4_ADDR(&ipaddr,  192, 168,   1, 100);		/* iperf server address */
#else
	IP4_ADDR(&ipaddr,  172, 16 ,   0, 100);		/* iperf server address */
#endif
	port = utxperf_port;                            /* iperf default port */
	err = udp_connect(pcb, &ipaddr, port);

	/* initialize data buffer being sent */
	for (i = 0; i < SEND_BUFSIZE; i++)
		send_buf[i] = (i % 10) + '0';

	connected_pcb = pcb;

#if !ALLOC_PBUF_ON_TRANSFER
	pbuf_to_be_sent = pbuf_alloc(PBUF_RAW, 1450, PBUF_POOL);
	if (!pbuf_to_be_sent)
		xil_printf("error allocating pbuf to send\r\n");
	else {
		memcpy(pbuf_to_be_sent->payload, send_buf, 1024);
		pbuf_to_be_sent->len = 1450;
	}
#endif

	return 0;
}

void
print_utxperf_app_header()
{
        xil_printf("%20s %6s %10s %s\r\n", "utxperf client",
                        "N/A",
                        txperf_client_connected ? "CONNECTED" : "CONNECTING",
                        "$ iperf -u -s -i 5 (on host with IP 192.168.1.100)");
}
