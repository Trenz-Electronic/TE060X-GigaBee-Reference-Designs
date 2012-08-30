# Reference projects for TE060X modules 
## Requirements: 
* Xilinx ISE 13.2 
* Xilinx EDK 13.2
* Xilinx ISE 14.2
* Xilinx EDK 14.2
* Git client

## Get projects
* Without Git
Go to the download page 
https://github.com/Trenz-Electronic/TE060X-GigaBee-Reference-Designs/downloads
Download project archive at the bottom of the page.
Unzip projects
Open project in Xilinx EDK or ISE

* Using Git
Run Git Bash
git clone git@github.com:Trenz-Electronic/TE060X-GigaBee-Reference-Designs.git
cd TE060X-GigaBee-Reference-Designs
git submodule init
git submodule update
Open project in Xilinx EDK or ISE

## Generic

In this folder you can find complete UCF file for module and baseboard.


## GigaBee_XPS13.2-Axi 

**﻿Recommended starting point for EDK-based designs**  

Hardware: Trenz Electronic TE0600 (GigaBee) XC6SLX45/100/150

Development environment: Xilinx XPS 13.2

This HW project is derived from SP605_AXI (Gigabit Ethernet, DMA, 100 MHz Microblaze) of Xilinx XAPP1026 "LightWeight IP (lwIP) Application Examples"
www.xilinx.com/support/documentation/application_notes/xapp1026.pdf
Software from XAPP1026 is fully compatible with this HW design.

Remarks:
+ PARAMETER C_USE_BRANCH_TARGET_CACHE was set to 0 to meet timing requirements.
+ When upgrading to XPS 13.3 or higher, we reccommend NOT to upgrade the AXI_ETHERNET core to v3.00.a, as we could not get it work!


## GigaBee_ISE13.2-Blinkin 

Hardware: Trenz Electronic TE0600 (GigaBee) XC6SLX45/100/150

Development environment: Xilinx ISE 13.2

* reference design: Clock and LED VHDL usage example
    good starting point for bistream generation options testing and SPI flash 
	booting sucesfully tested with x4 SPI mode, 80MHz SPI clock and compression 
	enabled


## GigaBee_ISE13.1-UDP_Datalogger

Hardware: Trenz Electronic TE0600 (GigaBee) XC6SLX45/100/150

Development environment: Xilinx ISE 13.1

This project demonstrates how to use the GigaBee module with its baseboard (equipped with an ethernet connector) as a data logger with 16 digital inputs and 16 digital outputs. Broadcast UDP packets are transmitted to a Gigabit Ethernet network.
Gigabit Ethernet only design, it does not work with 100 megabit Ethernet.

## GigaBee_ISE12.4-MIG

Hardware: Trenz Electronic TE0600 (GigaBee) XC6SLX45/100/150

Development environment: Xilinx ISE PN v12.4

Reference design: Memory Interface Generator 3.61 project ported to TE0600 (GigaBee)

## GigaBee_XPS13.2-AXI_lite 

Hardware: Trenz Electronic TE0600 (GigaBee) XC6SLX45/100/150

Development environment: Xilinx ISE XMP v13.2

Software from Xilinx XAPP1026 "LightWeight IP (lwIP) Application Examples"
www.xilinx.com/support/documentation/application_notes/xapp1026.pdf
is fully compatible with this HW design, derived from SP601_AXI reference design (10/100 EthernetLite, 100 MHz Microblaze).
This project work only with Ethernet 10/100 connection during to axi_ethernetlite limitations. If you have Gigabit NIC,
you should disable Gigabit ("Auto disable Gigabit" option in "Advansed" configuration) to make it work.

## GigaBee_XPS14.2-FlashWriter

Hardware: Trenz Electronic TE0600 (GigaBee) XC6SLX45/100/150

Development environment: Xilinx ISE XMP v14.2

FlashWriter is an application designed to demonstrate GigaBee FPGA module functionality.
It offers GigaBee module selftest routines and fast Ethernet FLASH writing. 
The selftest result is also reported on TE0603 LEDs, terminal and PC application.
The flashwriter SW executable is small in size can run from the 64kB BRAM! 
No bootloader is necessary.
