@rem Configure environment
@call C:\Xilinx\13.2\ISE_DS\settings64.bat

@rem Prepare lx45 project
@copy /y system.xmp.lx45 system.xmp
@copy /y system_incl.make.lx45 system_incl.make
@copy /y data\system.ucf.lx45 data\system.ucf
@rem Clean project
@make -f system.make hwclean
