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
 * 1wire.c
 *
 *      Author: Oleksandr Kiyenko
 */
# include "1wire.h"

#include <stdio.h>

static u8 crc8_table[] = {
        0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
      157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
       35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
      190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
       70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
      219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
      101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
      248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
      140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
       17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
      175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
       50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
      202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
       87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
      233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
      116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

/* Reset 1-wire bus
 * @param	Base address of axi_1wire instance
 * @return	0 - devises found on bus 1 - no devices
 */
int one_wire_reset(int base){
	volatile unsigned int *one_wire_core = (unsigned int*)base;
	one_wire_core[ONE_WIRE_CONTROL_REG_ADDR] = ONE_WIRE_CMD_RESET;
	while(one_wire_core[ONE_WIRE_STATUS_REG_ADDR] & ONE_WIRE_BUSY_MASK);
	return (one_wire_core[ONE_WIRE_STATUS_REG_ADDR] & ONE_WIRE_RES_MASK) >> 1;
}

/* Write 1 bit to 1-Wire bus
 * @param 	value == 0 -> write 0, != 0 -> write 1
 * @return	0
 */

int one_wire_write_bit(int base, u8 btw){
	volatile unsigned int *one_wire_core = (unsigned int*)base;
	one_wire_core[ONE_WIRE_CONTROL_REG_ADDR] = btw ? 1 : 0;
	while(one_wire_core[ONE_WIRE_STATUS_REG_ADDR] & ONE_WIRE_BUSY_MASK);
	return 0;
}

/* Write byte to 1-Wire bus
 * @param 	byte to write
 * @return	0
 */
int one_wire_write_byte(int base, u8 btw){
	int i;
	for(i = 0x01; i < 0x100; i = i << 1)
		one_wire_write_bit(base, btw & i);
	return 0;
}


/* Read 1 bit from 1-Wire bus
 * @param 	None
 * @return	Read result 1 or 0
 */
u8 one_wire_read_bit(int base){
	volatile unsigned int *one_wire_core = (unsigned int*)base;
	one_wire_core[ONE_WIRE_CONTROL_REG_ADDR] = ONE_WIRE_CMD_RD;
	while(one_wire_core[ONE_WIRE_STATUS_REG_ADDR] & ONE_WIRE_BUSY_MASK);
	return (one_wire_core[ONE_WIRE_STATUS_REG_ADDR] & ONE_WIRE_RES_MASK) >> 1;
}

/* Read byte from 1-Wire bus
 * @param 	None
 * @return	Read result
 */
u8 one_wire_read_byte(int base){
	u8 i, r;

	r = 0;
	for(i = 0; i < 8; i++){
		r = r >> 1;
		if(one_wire_read_bit(base))
			r |= 0x80;
	}
	return r;
}

/* Search for devices on 1-Wire bus
 * @param	device start address
 * @param	defined bits in address
 * @return	0		- No devices
 * 			255		- 1 device
 * 			1-64	- 1 device, but more than 1 can be found
 */
int one_wire_search(int base, unsigned char* start_addr, u8 defined){
	u8 t, min_cnt, d;
	int addr_bit;

	one_wire_write_byte(base, ONE_WIRE_CMD_SEARCH_ROM);	// Start search
	min_cnt = 0;
	for(addr_bit = 0; addr_bit < 64; addr_bit++){
		d = start_addr[addr_bit >> 3] & (1 << (addr_bit & 7));
		t = one_wire_read_bit(base);
		t = t << 4;
		t |= one_wire_read_bit(base);
		switch(t){
			case 0x00:	// Both
				if((addr_bit + 1) > defined){
					one_wire_write_bit(base, 0);	// Go to 0 branch
					start_addr[addr_bit >> 3] &= ~(1 << (addr_bit & 7));	// Store bit
					if(!min_cnt)
						min_cnt = addr_bit + 1;
				}
				else {	// Should go by defined way
					if(d){
						one_wire_write_bit(base, 1);	// Go to 1 branch
						start_addr[addr_bit >> 3] |= (1 << (addr_bit & 7));		// Store bit
					}
					else{
						one_wire_write_bit(base, 0);	// Go to 0 branch
						start_addr[addr_bit >> 3] &= ~(1 << (addr_bit & 7));	// Store bit
					}
				}
				break;
			case 0x01:	// Only 0s
				if((addr_bit < defined) && (d))
					return 0;
				one_wire_write_bit(base, 0);	// Only way is 0
				start_addr[addr_bit >> 3] &= ~(1 << (addr_bit & 7));	// Store bit
				break;
			case 0x10:	// Only 1s
				if((addr_bit < defined) && !(d))
					return 0;
				one_wire_write_bit(base, 1);	// Only way is 1
				start_addr[addr_bit >> 3] |= (1 << (addr_bit & 7));		// Store bit
				break;
			case 0x11:	// No dev
				return 0;
				break;
		}
	}
	if(min_cnt)
		return min_cnt;
	else
		return 255;
}

/* Search for devices from known family
 * use this function only if one defice from fimily present on bus.
 *
 * @param	device start address (low 8 bit is family ID)
 * @return	0		- No devices
 * 			1		- Device found
 */

int one_wire_family(int base, unsigned char* addrp){
	if(one_wire_reset(base)){
		return 0;	// No devices at all
	}
	one_wire_write_byte(base, ONE_WIRE_CMD_READ_ROM);
	one_wire_write_byte(base, addrp[0]);	// Send family ID
	addrp[1] = one_wire_read_byte(base);
	addrp[2] = one_wire_read_byte(base);
	addrp[3] = one_wire_read_byte(base);
	addrp[4] = one_wire_read_byte(base);
	addrp[5] = one_wire_read_byte(base);
	addrp[6] = one_wire_read_byte(base);
	addrp[7] = one_wire_read_byte(base);
	return 1;
}

/* Calculate CRC8
 * @param	old commulative CRC value
 * @param	value to add
 * @return	new CRC
 */
u8 one_wire_crc8(u8 ov, u8 av){
	return crc8_table[ov ^ av];
}
