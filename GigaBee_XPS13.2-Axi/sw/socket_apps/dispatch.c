/*
 * Copyright (c) 2009 Xilinx, Inc.  All rights reserved.
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

#include "lwip/inet.h"
#include "lwip/ip_addr.h"

#include "config_apps.h"

void
print_headers()
{
    xil_printf("\r\n");
    xil_printf("%20s %6s %s\r\n", "Server", "Port", "Connect With..");
    xil_printf("%20s %6s %s\r\n", "--------------------", "------", "--------------------");

    if (INCLUDE_ECHO_SERVER)
        print_echo_app_header();

    if (INCLUDE_RXPERF_SERVER)
        print_rxperf_app_header();

    if (INCLUDE_TXPERF_CLIENT)
        print_txperf_app_header();

    if (INCLUDE_TFTP_SERVER)
        print_tftp_app_header();

    if (INCLUDE_WEB_SERVER)
        print_web_app_header();

    xil_printf("\r\n");
}

void web_application_thread(void *);
void tx_application_thread(void *);
void rx_application_thread(void *);
void echo_application_thread(void *);
void tftpserver_application_thread(void *);

void
launch_app_threads()
{
    /* start webserver thread */
    if (INCLUDE_WEB_SERVER)
        sys_thread_new("httpd", web_application_thread, 0,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

    /* start echo server thread */
    if (INCLUDE_ECHO_SERVER)
        sys_thread_new("echod", echo_application_thread, 0,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

    /* start tftp server thread */
    if (INCLUDE_TFTP_SERVER)
        sys_thread_new("tftpd", tftpserver_application_thread, 0,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

    /* start echo server thread */
    if (INCLUDE_RXPERF_SERVER)
        sys_thread_new("rxperfd", rx_application_thread, 0,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

    /* start echo server thread */
    if (INCLUDE_TXPERF_CLIENT)
        sys_thread_new("txperfd", tx_application_thread, 0,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);
}
