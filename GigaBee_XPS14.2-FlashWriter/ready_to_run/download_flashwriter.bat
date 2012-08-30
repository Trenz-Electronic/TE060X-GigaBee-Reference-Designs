@echo off

rem **************************************************
rem * upload flash writer bitstream
rem **************************************************

:menu
echo.
echo.
echo ***************************************************
echo * Enter the number for the appropriate FPGA size: *
echo ***************************************************
echo.
echo 1. LX45
echo 2. LX100
echo 3. LX150
echo 0. exit
echo. 

set /p choice="Enter your choice: "
if %choice%==1 goto LX45
if %choice%==2 goto LX100
if %choice%==3 goto LX150
if %choice%==0 exit
echo Invalid choice: %choice%
echo.
goto menu

:LX45
echo.
echo.
echo *****************************************************************
echo * Programming module with flashwriter_LX45.bit...               *
echo *****************************************************************
echo.
echo.
impact -batch bin\download_flashwriter_LX45.cmd
goto buildbin

:LX100
echo.
echo.
echo *****************************************************************
echo * Programming module with flashwriter_LX100.bit...              *
echo *****************************************************************
echo.
echo.
impact -batch bin\download_flashwriter_LX100.cmd
goto buildbin

:LX150
echo.
echo.
echo *****************************************************************
echo * Programming module with flashwriter_LX150.bit...              *
echo *****************************************************************
echo.
echo.
impact -batch bin\download_flashwriter_LX150.cmd
goto buildbin


:buildbin





