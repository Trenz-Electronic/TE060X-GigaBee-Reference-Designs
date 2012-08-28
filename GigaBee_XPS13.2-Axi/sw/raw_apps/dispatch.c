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

void print_headers()
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

int
start_applications()
{
    if (INCLUDE_ECHO_SERVER)
        start_echo_application();

    if (INCLUDE_RXPERF_SERVER)
        start_rxperf_application();

    if (INCLUDE_TXPERF_CLIENT)
        start_txperf_application();

    if (INCLUDE_TFTP_SERVER)
        start_tftp_application();

    if (INCLUDE_WEB_SERVER)
        start_web_application();
}

int
transfer_data()
{
    if (INCLUDE_ECHO_SERVER)
        transfer_echo_data();

    if (INCLUDE_RXPERF_SERVER)
        transfer_rxperf_data();

    if (INCLUDE_TXPERF_CLIENT)
        transfer_txperf_data();

    if (INCLUDE_TFTP_SERVER)
        transfer_tftp_data();

    if (INCLUDE_WEB_SERVER)
        transfer_web_data();
}
