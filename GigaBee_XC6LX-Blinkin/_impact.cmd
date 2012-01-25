loadProjectFile -file "D:\Projekti\CAMOS\ISE\Blinkin\auto_project.ipf"
setMode -ss
setMode -sm
setMode -hw140
setMode -spi
setMode -acecf
setMode -acempm
setMode -pff
setMode -bs
setMode -bs
attachflash -position 1 -spi "W25Q64BV"
setMode -bs
setMode -bs
setMode -bs
setMode -pff
setMode -pff
setMode -pff
setMode -pff
setMode -pff
setCurrentDesign -version 0
setMode -pff
setCurrentDeviceChain -index 0
setSubmode -pffspi
setMode -pff
setMode -bs
setMode -bs
deleteDevice -position 1
setMode -bs
setMode -ss
setMode -sm
setMode -hw140
setMode -spi
setMode -acecf
setMode -acempm
setMode -pff
deletePromDevice -position 1
setCurrentDesign -version 0
deleteDevice -position 1
deleteDesign -version 0
setCurrentDesign -version -1
setMode -bs
setMode -bs
setMode -bs
setCable -port auto
Identify -inferir 
identifyMPM 
attachflash -position 1 -spi "W25Q64BV"
assignfiletoattachedflash -position 1 -file "D:/Projekti/CAMOS/ISE/Blinkin/blinkin.mcs"
Program -p 1 -dataWidth 4 -spionly -e -v -loadfpga 
assignFile -p 1 -file "D:/Projekti/CAMOS/ISE/Blinkin/blinkin.bit"
Program -p 1 
setMode -pff
setMode -pff
addConfigDevice  -name "blinkin_LX150" -path "D:\Projekti\CAMOS\ISE\Blinkin\"
setSubmode -pffspi
setAttribute -configdevice -attr multibootBpiType -value ""
addDesign -version 0 -name "0"
setMode -pff
addDeviceChain -index 0
setMode -pff
addDeviceChain -index 0
setAttribute -configdevice -attr compressed -value "FALSE"
setAttribute -configdevice -attr compressed -value "FALSE"
setAttribute -configdevice -attr autoSize -value "TRUE"
setAttribute -configdevice -attr fileFormat -value "mcs"
setAttribute -configdevice -attr fillValue -value "FF"
setAttribute -configdevice -attr swapBit -value "FALSE"
setAttribute -configdevice -attr dir -value "UP"
setAttribute -configdevice -attr multiboot -value "FALSE"
setAttribute -configdevice -attr multiboot -value "FALSE"
setAttribute -configdevice -attr spiSelected -value "TRUE"
setAttribute -configdevice -attr spiSelected -value "TRUE"
setMode -bs
setMode -bs
setMode -bs
setMode -pff
setMode -pff
setMode -pff
setMode -pff
addDeviceChain -index 0
setMode -pff
addDeviceChain -index 0
setSubmode -pffspi
setMode -pff
setAttribute -design -attr name -value "0000"
addDevice -p 1 -file "D:/Projekti/CAMOS/ISE/Blinkin/blinkin.bit"
setMode -pff
setSubmode -pffspi
generate -generic
setCurrentDesign -version 0
setMode -bs
setMode -bs
setMode -bs
attachflash -position 1 -spi "W25Q64BV"
assignfiletoattachedflash -position 1 -file "D:/Projekti/CAMOS/ISE/Blinkin/blinkin_LX150.mcs"
Program -p 1 -dataWidth 4 -spionly -e -v -loadfpga 
assignFile -p 1 -file "D:/Projekti/CAMOS/ISE/Blinkin/blinkin.bit"
assignFile -p 1 -file "D:/Projekti/CAMOS/ISE/Blinkin/blinkin.bit"
assignFile -p 1 -file "D:/Projekti/CAMOS/ISE/Blinkin/blinkin.bit"
setMode -bs
setMode -bs
setMode -ss
setMode -sm
setMode -hw140
setMode -spi
setMode -acecf
setMode -acempm
setMode -pff
setMode -bs
saveProjectFile -file "D:/Projekti/CAMOS/ISE/Blinkin/auto_project.ipf"
setMode -bs
setMode -pff
setMode -bs
deleteDevice -position 1
setMode -bs
setMode -ss
setMode -sm
setMode -hw140
setMode -spi
setMode -acecf
setMode -acempm
setMode -pff
deletePromDevice -position 1
setCurrentDesign -version 0
deleteDevice -position 1
deleteDesign -version 0
setCurrentDesign -version -1
