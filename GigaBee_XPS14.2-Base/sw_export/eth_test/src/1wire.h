/*
Copyright (C) 2012 Trenz Electronic

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/
/*
 * 1wire.h
 *
 *      Author: Oleksandr Kiyenko
 */
#include "xbasic_types.h"

#ifndef ONE_WIRE_H_
#define ONE_WIRE_H_

#define ONE_WIRE_STATUS_REG_ADDR	0
#define ONE_WIRE_CONTROL_REG_ADDR	1

#define ONE_WIRE_CMD_RESET			3
#define ONE_WIRE_CMD_WR0			0
#define ONE_WIRE_CMD_WR1			1
#define ONE_WIRE_CMD_RD				2

#define ONE_WIRE_BUSY_MASK			1
#define ONE_WIRE_RES_MASK			2

#define ONE_WIRE_CMD_SKIP_ROM		0xCC
#define ONE_WIRE_CMD_READ_ROM		0x33
#define ONE_WIRE_CMD_MATCH_ROM		0x55
#define ONE_WIRE_CMD_SEARCH_ROM		0xF0

#define DS2502_CMD_READ_MEMORY		0xF0
#define DS2502_CMD_READ_STATUS		0xAA
#define DS2502_CMD_READ_DATA		0xC3
#define DS2502_CMD_WRITE_MEMORY		0x0F
#define DS2502_CMD_WRITE_STATUS		0x55

int one_wire_reset(int base);
int one_wire_write_bit(int base, u8 btw);
int one_wire_write_byte(int base, u8 btw);
u8 one_wire_read_bit(int base);
u8 one_wire_read_byte(int base);
int one_wire_search(int base, unsigned char* addrp, u8 defined);
int one_wire_family(int base, unsigned char* addrp);
u8 one_wire_crc8(u8 ov, u8 av);

#endif /* ONE_WIRE_H_ */
