@echo off


rem **************************************************
rem * build binary from bitstream 'fpga.bit'
rem ***************************************0***********

echo.
echo.
echo *****************************************************************
echo * Converting 'download.bit' bitstream to 'fpga.bin' bin file... *
echo *****************************************************************
echo.
echo.

if exist download.bit impact -batch bin\bit2mcs.cmd

if exist fpga.mcs bin\xmcsutil -accept_notice -1 -i fpga.mcs -o fpga.hex
if exist fpga.hex bin\xmcsutil -accept_notice -18 -i fpga.hex -o fpga.bin

echo.
echo.
echo *****************************************************************
echo * Uploading 'fpga.bin' into flash via ethernet...               *
echo *****************************************************************
echo.
echo.

bin\gigabeeflash fpga.bin


rem if exist fpga.mcs del fpga.mcs
if exist fpga.hex del fpga.hex
if exist fpga.cfi del fpga.cfi
if exist fpga.prm del fpga.prm
rem if exist fpga.bin del fpga.bin
if exist _impactbatch.log del _impactbatch.log
if exist download_cclktemp.bit del download_cclktemp.bit

echo.
echo.


