@rem Configure environment
@call C:\Xilinx\13.2\ISE_DS\settings64.bat

@rem Prepare lx100 project
@copy /y system.xmp.lx100 system.xmp
@copy /y system_incl.make.lx100 system_incl.make
@copy /y data\system.ucf.lx100 data\system.ucf
@rem Clean project
@make -f system.make hwclean
