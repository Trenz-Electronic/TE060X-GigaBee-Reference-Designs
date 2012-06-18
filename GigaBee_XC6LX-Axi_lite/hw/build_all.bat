rem Configure environment
call C:\Xilinx\13.2\ISE_DS\settings64.bat

@rem Prepare lx45 project
@rem @copy /y system.xmp.lx45 system.xmp
@rem @copy /y system_incl.make.lx45 system_incl.make
@rem @copy /y data\system.ucf.lx45 data\system.ucf
@rem Clean project
@rem make -f system.make hwclean
@rem Generate bitstream
@rem make -f system.make init_bram
@rem Copy result
copy /y implementation\system.bit system_lx45.bit
copy /y implementation\system_bd.bmm system_bd_lx45.bmm
copy /y implementation\download.bit download_lx45.bit
@rem Clean project
@rem make -f system.make hwclean


@rem Generate FWU for LX45
@copy download_lx45.bit fpga.bit
@impact -batch etc\bit2mcs.cmd
@move fpga.mcs TE0600-LX45.mcs
@rem Remove logs
@del fpga.bin fpga.prm fpga.cfi fpga.bit fpga.sig


@rem Remove files
@del usb.bin 
@del Bootload.ini
@del _impactbatch.log

pause