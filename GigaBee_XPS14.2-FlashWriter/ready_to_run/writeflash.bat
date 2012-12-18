
echo.
echo.
echo *****************************************************************
echo * Uploading 'fpga.bin' into flash via ethernet...               *
echo *****************************************************************
echo.
echo.

bin\gigabeeflash fpga.bin
pause

rem if exist fpga.mcs del fpga.mcs
if exist fpga.hex del fpga.hex
if exist fpga.cfi del fpga.cfi
if exist fpga.prm del fpga.prm
rem if exist fpga.bin del fpga.bin
if exist _impactbatch.log del _impactbatch.log
if exist download_cclktemp.bit del download_cclktemp.bit

echo.
echo.


