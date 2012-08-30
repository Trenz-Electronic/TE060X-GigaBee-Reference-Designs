# GigaBee FlashWriter project
## Requrements:
* Xilinx XPS 14.2
* TE0600 module
* TE0603 baseboard

## Project description
FlashWriter is an application designed to demonstrate GigaBee FPGA module functionality.
It offers GigaBee module selftest routines and fast Ethernet FLASH writing. 
The selftest result is also reported on TE0603 LEDs, terminal and PC application.
LEDs status:
Initially all LEDs off
<table>
<th>
<td>LED</td><td>LED location</td>
</th>
<tr>
<td>Timer OK</td><td>LED 0 ON</td><td>(on module)</td>
</tr>
<tr>
<td>Both RAMs OK</td><td>LED 1 ON</td><td>(on base board)</td>
</tr>
<tr>
<td>Flash OK</td><td>LED 2 ON</td><td>(on base board)</td>
</tr>
<tr>
<td>EEPROM OK</td><td>LED 3 ON</td><td>(on base board)</td>
</tr>
<tr>
<td>Ethernet OK</td><td>LED 4 ON</td><td>(on base board)</td>
</tr>
</table>

The design constins all the necessary source files and we recommend this as a 
starting point for new designs.
The HW design is also compatible to Xilinx XAPP1026 Webserver reference design.

- - -

> * NOTE :*
> Project use axi_ethernet core, so if you have only evaluation
> license, Ethernet will work only limited amount of time.

## Folders contents
- **host_sw_src** - Sources for gigabeeflash.exe executable on PC side. The project files are for Qt SDK v2010 or later.
- **hw** - Xilinx EDK/SDK v14.2 project including HW design.
- **ready_to_run** - Binaries for immediate use.
- **SDK** - Firmware projects and IP repository

## Project build
- Convert project for your module.
Run hw\set_lx45_project.bat if your module have LX45 chip.
Run hw\set_lx100_project.bat if your module have LX100 chip.
Run hw\set_lx150_project.bat if your module have LX150 chip.
- Run Xilinx Platform Studio and open system.xmp in "hw" folder.
- Run "Hardware"->"Generate Bitstream"
- Run "Project"->"Export Hardware Design to SDK..."
- select "Export & Launch SDK"
- select "SDK\SDK_Workspace" as Workspace folder

The flashwriter SW executable is small in size can run from the 64kB BRAM! 
No bootloader is necessary.