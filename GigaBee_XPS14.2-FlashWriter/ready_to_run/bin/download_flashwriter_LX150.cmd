setMode -bscan
setCable -p auto
identify
assignfile -p 1 -file bin\flashwriter_LX150.bit
program -p 1
quit
