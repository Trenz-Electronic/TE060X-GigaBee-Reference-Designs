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
