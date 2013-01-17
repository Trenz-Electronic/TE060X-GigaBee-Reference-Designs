setMode -bs
setMode -bs
setMode -bs
setMode -bs
setCable -port auto
identify
attachflash -position 1 -spi "W25Q64FV"
assignfiletoattachedflash -position 1 -file "bootloader.mcs"
Program -p 1 -dataWidth 1 -spionly
quit
