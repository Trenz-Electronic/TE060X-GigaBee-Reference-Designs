GigaBee FlashWriter application
Usage:
1. Set PC IP to automatic private address. This sets PC IP to 169.254.XXX.XXX, mask 255.255.0.0. FPGA IP is set to 169.254.169.XXX.
2. Make sure that you have Xilinx JTAG cable tools available from command line (run cmd and type impact to verify).
3. Put the GigaBee (TE0600) module to TE0603 base board.
4. Connect FPGA board to Ethernet with PC, JTAG and optionally UART terminal on PMOD connector (TX from FPGA is PIN A17). UART settings are 115200bps, 8bits, 1stop bit, no parity.
5. Run download_flashwriter.bat and select proper FPGA size
6. Put your bitstream in this folder and rename it to "download.bit". Optionally you can use "fpga.mcs" or "fpga.bin".
7. Then run writeflash.bat
8. Wait for the FPGA to reboot

NOTES:
1. If you do not have command line tools setup you can always run the iMPACT in GUI mode. Select bin\flashwriter_XXX.bit as a bistream and program the FPGA. Then run writeflash.bat
