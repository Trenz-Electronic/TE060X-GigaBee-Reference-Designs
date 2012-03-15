= Reference projects for TE060X modules =
== Requirements: ==
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

--------------------------------------------------------------------------------
== GigaBee_XC6LX-AXI ==
'''BEST STARTING POINT FOR EDK BASED DESIGN'''
* hardware: Trenz Electronic GigaBee XC6SLX100
* reference design: Xilinx XAPP1026, derived from sp605_Axi (Gigabit & DMA), 
	Microblaze clock 100MHz
	Warning: The design without license for AXI_Ethernet will function only 
	8 hours
	Software from XAPP1026 is fully compatible with this HW design
	To meet timing the  PARAMETER C_USE_BRANCH_TARGET_CACHE was set to 0
* read documentation at 
	http://www.xilinx.com/support/documentation/application_notes/xapp1026.pdf
* development environment: Xilinx ISE XMP v13.2
* When upgrading to ISE XMP v13.3 or higher, do not upgrade the AXI_ETHERNET core to 
	v3.00.a. We could not make it work!
* Reimplement to specific FPGA size (LX45, LX150) with setting the proper FPGA 
	in project options and 
* UNCOMMENT the proper IDELAY_VALUE in the begining of the data/system.ucf file 
	to meet timing!

== GigaBee_XC6LX-Blinkin ==
* hardware: Trenz Electronic GigaBee XC6SLX
* reference design: Clock and LED VHDL usage example
    good starting point for bistream generation options testing and SPI flash 
	booting sucesfully tested with x4 SPI mode, 80MHz SPI clock and compression 
	enabled
* development environment: Xilinx ISE PN v13.2

== GigaBee_UDP_Datalogger ==
* hardware: Trenz Electronic GigaBee XC6SLX45
* development environment: Xilinx ISE XMP v13.1
This project demonstrates how to use GigaBee board
together with baseboard equipped by ethernet connector as
a data logger with 16 digital inputs and output in form
of broadcast UDP ethernet packets transmitted to Gigabit
Ethernet network (Gigabit only. Doesn't work for 100Mbps.).

== GigaBee_XC6SLX-MIG ==
* hardware: Trenz Electronic GigaBee XC6SLX
* reference design: Modified Memory Interface Generator 3.61 project to fit 
	GigaBee
* development environment: Xilinx ISE PN v12.4

---------------------------------------------------------------------------------
Projects under active development - not recomended for new users
---------------------------------------------------------------------------------
== GigaBee_XC6LX-AXI_lite ==
* hardware: Trenz Electronic GigaBee XC6SLX100
* reference design: Xilinx XAPP1026, derived from sp601_Axi 
	(10/100 EthernetLite), Microblaze clock 100MHz
	Software from XAPP1026 is fully compatible with this HW design
* read documentation at 
	http://www.xilinx.com/support/documentation/application_notes/xapp1026.pdf
* development environment: Xilinx ISE XMP v13.1
* Does not work!!!Problems with software 
	(we suspect Ethernet PHY address is wrong)!!
	
