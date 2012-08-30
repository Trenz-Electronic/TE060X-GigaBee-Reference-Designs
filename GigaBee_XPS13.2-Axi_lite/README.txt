Copyright (C) 2012 Trenz Electronic

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
IN THE SOFTWARE.
--------------------------------------------------------------------------------
Reference AXI-lite project for GigaBee TE0600 boards.

Project is under MIT license. All contributions should mit MIT license.
--------------------------------------------------------------------------------
# GigaBee_XC6LX-AXI_lite 

Hardware: Trenz Electronic TE0600 (GigaBee) XC6SLX100

Development environment: Xilinx ISE XMP v13.2

Software from Xilinx XAPP1026 "LightWeight IP (lwIP) Application Examples"

www.xilinx.com/support/documentation/application_notes/xapp1026.pdf

is fully compatible with this HW design, derived from SP601_AXI reference design (10/100 EthernetLite, 100 MHz Microblaze).

##Note

This project work only with Ethernet 10/100 connection during to axi_ethernetlite limitations. If you have Gigabit NIC,
you should disable Gigabit ("Auto disable Gigabit" option in "Advansed" configuration) to make it work.

Build:
- convert project for your module:
  run hw\set_lx45_project.bat for module with XC6SLX45
  run hw\set_lx100_project.bat for module with XC6SLX100
  run hw\set_lx150_project.bat for module with XC6SLX150
- open hw\system.xmp using Xilinx EDK 32.2
  In Xilinx EDK
- run "Hardware -> Generate Bitstream"
- run "Project -> Export Hardware Design to SDK..."
- select "Export & Launch SDK"
- specify place for new workspace and press "OK"
  In Xilinx SDK
- select "Xilinx Tools -> Repositories" 
- press "New" near "Local Repositories" area
- navigate to "Repository" in project directory 
- Press "Rescan Repositories" and then "OK"
- right click on SDK "Project explorer" panel and select "Import..."
- select "General -> Existing Project into Workspace"
- click "Browse" button near "Select root directory" navigate to project sw directory
- select "raw_apps" and "raw_bsp" projects and check "Copy projects into workspace" and click "Finish"
- run "Project -> Build all"
Test:
- connect module to PC using Xilinx JTAG cable and Ethernet cable
- power on module
- run "Control Panel" and click to "View network status and tasks"
- click  "Local area connection"
- go to IPv4 settings and set IPv4 IP address to 192.168.1.2 netmask 255.255.255.0
- apply new IPs
- if you have Gigabit NIC you should put it to 10/100 mode
  At "Local area connection status" press "Properties" then "Configure"
  select "Advansed"
  set "Auto disable Gigabit" to "Re-Link. Battery or AC"
- run "Xilnx Tools -> Program FPGA"
- run "Xilinx Tools -> XMD Console"
  In XMD Console tab
* connect mdm -uart
* terminal -jatg_uart_server 4321
* connect terminal program to 127.0.0.1:4321 to see program output
- cd ../ready_for_download
- connect mb mdm
- dow -data image.mfs 0xC5000000
  In SDK window
- select "raw_apps" on "Project Explorer" tab, right click and select "Run As -> Launch on Hardware"
- wait while program boot (around 30 seconds)
- run browser and go to 192.168.1.10 to see web server aplication output

* steps is iptional