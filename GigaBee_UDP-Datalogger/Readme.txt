This project demonstrates how to use GigaBee board
together with baseboard equipped by ethernet connector as
a data logger with 16 digital inputs and output in form
of broadcast UDP ethernet packets transmitted to Gigabit
Ethernet network (Gigabit only. Doesn't work for 100Mbps.).

The project has two parts. HW part, design for Spartan 6
FPGA of GigaBee board, and SW part, simple PC application
that is able to catch packets from GigaBee board and write
the date to the disk file.

Compiled bitstream for FPGA can be found in:
!Exacutables!\GigaBee_udp_data_logger.bit

Software application that can catch data can be found compiled in
(Data are logged in binary form. Packet are saved back to back. 
Each packet contains 16 Bytes of header data and 1280 Bytes of 
logged event records - each record 4 Byte timestamp + 2 Bytes data.
For exact structure, see description in top.vhd.):
!Executables!\UDP_Data_Logger_Receiver_PC.exe

In this default bitstream, internally (pseudo - random) data generator 
is connected to data logger and high data rate is generated (approx. 
30% bandwidth). Bandwidth is intentionally limited not to disrupt
any network, but anyway it is highly recommended to connect this 
example to local networks only.

Platform independent source files for HW project can be found in
src folder. Platform specific part is placed in 
xilinx_gigabee_gmii_test folder that contains complete ISE13.1
project.

Folder sw contains simple PC application written in C that catches
the data from logger and saves it to disk file.

For further project details and licenses see source files.
If you have any questions to this project please feel free to
contact author at:
info@dfcdesign.cz


