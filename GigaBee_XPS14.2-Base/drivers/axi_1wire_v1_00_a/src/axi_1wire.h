/*****************************************************************************
* Company:			 Trenz Electronic
* Author			 Oleksandr Kiyenko
* Filename:          axi_1wire.h
* Version:           1.00.a
* Description:       axi_1wire Driver Header File
* Date:              Wed Dec 26 13:05:06 2012
*****************************************************************************/

#ifndef AXI_1WIRE_H
#define AXI_1WIRE_H

/***************************** Include Files *******************************/

#include "xbasic_types.h"
#include "xstatus.h"
#include "xil_io.h"

/************************** Constant Definitions ***************************/

#define AXI_1WIRE_STATUS_REG_OFFSET			0x00000000
#define AXI_1WIRE_CONTROL_REG_OFFSET		0x00000004

#define AXI_1WIRE_BUSY_MASK					0x00000001
#define AXI_1WIRE_RES_MASK					0x00000002

#define AXI_1WIRE_RST_CMD					0x00000003
#define AXI_1WIRE_WR0_CMD					0x00000000
#define AXI_1WIRE_WR1_CMD					0x00000001
#define AXI_1WIRE_RD_CMD					0x00000002

/************************** Function Prototypes ****************************/
u32 one_wire_reset(u32 base_address);
u32 one_wire_write_bit(u32 base_address, u32 data);
u32 one_wire_read_bit(u32 base_address);
u32 one_wire_wait(u32 base_address);

#endif /** AXI_1WIRE_H */
