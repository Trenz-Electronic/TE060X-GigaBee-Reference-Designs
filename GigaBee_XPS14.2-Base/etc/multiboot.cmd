setMode -pff
setMode -pff
addConfigDevice  -name "fpga" -path ".\"
setSubmode -pffspi
setAttribute -configdevice -attr multibootBpiType -value "TYPE_MB_SPI"
setAttribute -configdevice -attr multibootBpiDevice -value "SPARTAN6"
setAttribute -configdevice -attr multibootBpichainType -value ""
setAttribute -configdevice -attr dir -value "UP"
addDesign -version 0 -name "0"
setAttribute -configdevice -attr compressed -value "FALSE"
setAttribute -configdevice -attr compressed -value "FALSE"
setAttribute -configdevice -attr autoSize -value "FALSE"
setAttribute -configdevice -attr fileFormat -value "mcs"
setAttribute -configdevice -attr fillValue -value "FF"
setAttribute -configdevice -attr swapBit -value "FALSE"
setAttribute -configdevice -attr dir -value "UP"
setAttribute -configdevice -attr dir -value "UP"
setAttribute -configdevice -attr multiboot -value "TRUE"
setAttribute -configdevice -attr multiboot -value "TRUE"
setAttribute -configdevice -attr spiSelected -value "TRUE"
setAttribute -configdevice -attr spiSelected -value "TRUE"
setAttribute -configdevice -attr ironhorsename -value "1"
setAttribute -configdevice -attr flashDataWidth -value "8"
setCurrentDesign -version 0
setAttribute -design -attr RSPin -value ""
setCurrentDesign -version 0
addPromDevice -p 1 -size 8192 -name 8M
setMode -pff
addDeviceChain -index 0
setSubmode -pffmbspi
setMode -pff
setAttribute -design -attr name -value "0000"
addDevice -p 1 -file "bit_a.bit"
setAttribute -design -attr name -value "0000"
setAttribute -design -attr endAddress -value "16acf0"
addDesign -version 1 -name "200000"
setMode -pff
addDeviceChain -index 0
setCurrentDesign -version 1
addDevice -p 1 -file "bit_b.bit"
setMode -pff
setSubmode -pffmbspi
generate
quit
