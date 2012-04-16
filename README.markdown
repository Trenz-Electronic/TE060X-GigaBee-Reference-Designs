# Reference projects for TE060X modules 
## Requirements: 
Xilinx ISE 13.2 
Xilinx EDK 13.2
Git client

To get projects:

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


## GigaBee_XC6LX-AXI 
**﻿Recommended starting point for EDK-based designs**  

Hardware: Trenz Electronic TE0600 (GigaBee) XC6SLX100

Development environment: Xilinx ISE XMP v13.2

This HW project is derived from SP605_AXI (Gigabit Ethernet, DMA, 100 MHz Microblaze) of Xilinx XAPP1026 "LightWeight IP (lwIP) Application Examples"

www.xilinx.com/support/documentation/application_notes/xapp1026.pdf

Software from XAPP1026 is fully compatible with this HW design.

Remarks:

+ PARAMETER C_USE_BRANCH_TARGET_CACHE was set to 0 to meet timing requirements.
+ When upgrading to ISE XMP v13.3 or higher, we reccommend NOT to upgrade the AXI_ETHERNET core to v3.00.a, as we could not get it work!
+ Re-implement to specific FPGA device (LX45, LX150) by

1. setting the proper FPGA device in project options and
2. uncommenting the corresponding IDELAY_VALUE at the beginning of data/system.ucf file!



## GigaBee_XC6LX-Blinkin 
* hardware: Trenz Electronic GigaBee XC6SLX
* reference design: Clock and LED VHDL usage example
    good starting point for bistream generation options testing and SPI flash 
	booting sucesfully tested with x4 SPI mode, 80MHz SPI clock and compression 
	enabled
* development environment: Xilinx ISE PN v13.2


## GigaBee_UDP_Datalogger
Hardware: Trenz Electronic GigaBee XC6SLX45

development environment: Xilinx ISE XMP v13.1

This project demonstrates how to use the GigaBee module with its baseboard (equipped with an ethernet connector) as a data logger with 16 digital inputs and 16 digital outputs. Broadcast UDP packets are transmitted to a Gigabit Ethernet network.

Gigabit Ethernet only design, it does not work with 100 megabit Ethernet.


## GigaBee_XC6SLX-MIG
Hardware: Trenz Electronic TE0600) GigaBee XC6SLX

Reference design: Memory Interface Generator 3.61 project ported to TE0600 (GigaBee)

Development environment: Xilinx ISE PN v12.4

- - - 
**Projects under active development - not recomended for new users**


## GigaBee_XC6LX-AXI_lite 

Hardware: Trenz Electronic TE0600 (GigaBee) XC6SLX100

Development environment: Xilinx ISE XMP v13.1

Software from Xilinx XAPP1026 "LightWeight IP (lwIP) Application Examples"

www.xilinx.com/support/documentation/application_notes/xapp1026.pdf

is fully compatible with this HW design, derived from SP601_AXI reference design (10/100 EthernetLite, 100 MHz Microblaze).

Does not work!!!Problems with software (we suspect Ethernet PHY address is wrong)!!
