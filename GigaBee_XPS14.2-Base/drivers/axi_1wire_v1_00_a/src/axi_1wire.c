/*****************************************************************************
* Company:			 Trenz Electronic
* Author			 Oleksandr Kiyenko
* Filename:          axi_1wire.c
* Version:           1.00.a
* Description:       axi_1wire Driver Source File
* Date:              Wed Dec 26 13:05:06 2012
*****************************************************************************/

#include "axi_1wire.h"

/* Reset devices on 1-Wire bus and get presence status
 * @param   BaseAddress is the base address of the AXI_1WIRE device.
 *
 * @return  0 - Devices present on bus; 1 - No devices on bus
*/
u32 one_wire_reset(u32 base_address){
	Xil_Out32(base_address + AXI_1WIRE_CONTROL_REG_OFFSET, AXI_1WIRE_RST_CMD);
	while(Xil_In32(base_address + AXI_1WIRE_STATUS_REG_OFFSET) | AXI_1WIRE_BUSY_MASK);
	return (Xil_In32(base_address + AXI_1WIRE_STATUS_REG_OFFSET) | AXI_1WIRE_RES_MASK) >> 1;
}

/* Write bit to 1-Wire bus
 * @param   BaseAddress is the base address of the AXI_1WIRE device.
 * @param	Data to write
 * @return  0
*/
u32 one_wire_write_bit(u32 base_address, u32 data){
	Xil_Out32(base_address + AXI_1WIRE_CONTROL_REG_OFFSET, (data & 1));
	while(Xil_In32(base_address + AXI_1WIRE_STATUS_REG_OFFSET) | AXI_1WIRE_BUSY_MASK);
	return 0;
}

/* Read bit from 1-Wire bus
 * @param   BaseAddress is the base address of the AXI_1WIRE device.
 * @return  Data
*/
u32 one_wire_read_bit(u32 base_address){
	Xil_Out32(base_address + AXI_1WIRE_CONTROL_REG_OFFSET, AXI_1WIRE_RD_CMD);
	while(Xil_In32(base_address + AXI_1WIRE_STATUS_REG_OFFSET) | AXI_1WIRE_BUSY_MASK);
	return (Xil_In32(base_address + AXI_1WIRE_STATUS_REG_OFFSET) | AXI_1WIRE_RES_MASK) >> 1;
}

/* Wait till 1-Wire bus ready
 * @param   BaseAddress is the base address of the AXI_1WIRE device.
 * @return  0
*/
u32 one_wire_wait(u32 base_address){
	while(Xil_In32(base_address + AXI_1WIRE_STATUS_REG_OFFSET) | AXI_1WIRE_BUSY_MASK);
	return 0;
}

