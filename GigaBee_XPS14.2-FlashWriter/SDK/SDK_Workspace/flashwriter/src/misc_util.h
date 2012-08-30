/*
 * misc_util.h
 *
 *  Created on: Dec 16, 2011
 *      Author: Ziga Lenarcic
 */

#ifndef MISC_UTIL_H_
#define MISC_UTIL_H_
#include <stdio.h>
#include "xstatus.h"

// debug print
#define DBG_PRINT 1
#define debug_print(fmt, ...) \
        do { if (DBG_PRINT) xil_printf("%s:%d:%s(): " fmt "\r\n", __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)

void util_16pattern(char *start, u32 length)
{
	unsigned BytesLeft = length;
	u8 *Frame;
	u16 Counter = 0;

	/*
	 * Set the frame pointer to the start of the payload area
	 */
	Frame = (u8 *)start;

	/*
	 * Insert 8 bit incrementing pattern
	 */
	while (BytesLeft && (Counter < 256)) {
		*Frame++ = (u8)Counter++;
		BytesLeft--;
	}

	/*
	 * Switch to 16 bit incrementing pattern
	 */
	while (BytesLeft) {
		*Frame++ = (u8) (Counter >> 8);	/* high */
		BytesLeft--;

		if (!BytesLeft)
			break;

		*Frame++ = (u8) Counter++;	/* low */
		BytesLeft--;
	}
}

void util_8_pattern(char *start, u32 length)
{
	u32 i;
	u8 *Frame = (u8 *)start;
	for (i = 0; i < length; i++)
	{
		*Frame++ = (u8)i;
	}
}

int util_compare_pattern(char *start, u32 length)
{
	u8 *dat = (u8 *)start;
	xil_printf("Comparing memory (address 0x%08x) with pattern for %d bytes...\r\n", (u32) start, length);

	u32 i;
	for (i = 0; i < length; i++)
	{
		if ((dat[i]) != (u8)(i&0xFF))
		{
			xil_printf("Not matching pattern at byte %d (address 0x%08x)!\r\n", i , i + (u32)start);
			return XST_FAILURE;
		}
	}
	xil_printf("Pattern matching!\r\n");
	return XST_SUCCESS;
}

void print_hex_array(const unsigned char *src, u32 length)
{
	int i;
	xil_printf("printing array at 0x%08x (%d bytes): \r\n", (u32)src, length);
	for (i = 0; i < length; i++) {
		if ((i % 16) == 0) xil_printf("\r\n [0x%08X]: ", (u32)src+i);
		xil_printf("0x%02x ", src[i] & 0xff);
	}
	xil_printf("\r\n");
}

void util_advance_spinner(void) {
    static char bars[] = { '/', '-', '\\', '|' };
    static int nbars = sizeof(bars) / sizeof(char);
    static int pos = 0;

    //usleep(1000);
    xil_printf("%c\r", bars[pos]);
    pos = (pos + 1) % nbars;
}


#endif /* MISC_UTIL_H_ */
