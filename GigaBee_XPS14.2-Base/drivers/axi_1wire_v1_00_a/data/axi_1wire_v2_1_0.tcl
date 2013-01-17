##############################################################################
## Filename:          B:\SVN\CustomerProjects\Schleich\S-Analyzer\current\hdl\k-boot/drivers/axi_1wire_v1_00_a/data/axi_1wire_v2_1_0.tcl
## Description:       Microprocess Driver Command (tcl)
## Date:              Wed Dec 26 13:05:06 2012 (by Create and Import Peripheral Wizard)
##############################################################################

#uses "xillib.tcl"

proc generate {drv_handle} {
  xdefine_include_file $drv_handle "xparameters.h" "axi_1wire" "NUM_INSTANCES" "DEVICE_ID" "C_BASEADDR" "C_HIGHADDR" 
}
