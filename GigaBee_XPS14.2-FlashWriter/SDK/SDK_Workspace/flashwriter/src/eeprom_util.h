/*
 * eeprom_util.h
 *
 *  Created on: Dec 27, 2011
 *      Author: Ziga Lenarcic
 */

#ifndef EEPROM_UTIL_H_
#define EEPROM_UTIL_H_

/// utilities for reading out MAC address bytes from single wire eeprom

#include "xparameters.h"
#include "onewire.h" // contains OneWireWrite/Read/Reset()

#define ONEWIRE_BASE_ADDR XPAR_ONEWIRE_EEPROM_BASEADDR  // the base addres of the device

// rename functions from onewire.h
#define one_wire_write(x) OneWireWrite(ONEWIRE_BASE_ADDR, x)
#define one_wire_read() OneWireRead(ONEWIRE_BASE_ADDR)
#define one_wire_reset() OneWireReset(ONEWIRE_BASE_ADDR)




/**********************************************************************
 *
 * Filename:    crc.c
 *
 * Description: Slow and fast implementations of the CRC standards.
 *
 * Notes:       The parameters for each supported CRC standard are
 *				defined in the header file crc.h.  The implementations
 *				here should stand up to further additions to that list.
 *
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

/*
 * Derive parameters from the standard-specific parameters in crc.h.
 */
typedef unsigned short uint16_t;
#define REFLECT_DATA(X)			((unsigned char) reflect((X), 8))
#define REFLECT_REMAINDER(X)	((uint16_t) reflect((X), 16))


/*********************************************************************
 *
 * Function:    reflect()
 *
 * Description: Reorder the bits of a binary sequence, by reflecting
 *				them about the middle position.
 *
 * Notes:		No checking is done that nBits <= 32.
 *
 * Returns:		The reflection of the original data.
 *
 *********************************************************************/
static unsigned long reflect(unsigned long data, unsigned char nBits)
{
	unsigned long  reflection = 0x00000000;
	unsigned char  bit;

	/*
	 * Reflect the data about the center bit.
	 */
	for (bit = 0; bit < nBits; ++bit)
	{
		/*
		 * If the LSB bit is set, set the reflection of it.
		 */
		if (data & 0x01)
		{
			reflection |= (1 << ((nBits - 1) - bit));
		}

		data = (data >> 1);
	}

	return (reflection);

}	/* reflect() */


uint16_t crc16(unsigned char const message[], int nBytes)
{
	uint16_t       remainder = 0x0000;
	int            byte;
	unsigned char  bit;


	/*
	 * Perform modulo-2 division, a byte at a time.
	 */
	for (byte = 0; byte < nBytes; ++byte)
	{
		/*
		 * Bring the next byte into the remainder.
		 */
		remainder ^= (REFLECT_DATA(message[byte]) << 8);

		/*
		 * Perform modulo-2 division, a bit at a time.
		 */
		for (bit = 8; bit > 0; --bit)
		{
			/*
			 * Try to divide the current data bit.
			 */
			if (remainder & 0x8000)
			{
				remainder = (remainder << 1) ^ 0x8005;
			}
			else
			{
				remainder = (remainder << 1);
			}
		}
	}

	/*
	 * The final remainder is the CRC result.
	 */
	return (REFLECT_REMAINDER(remainder) );

}   /* crcSlow() */

typedef struct {
	unsigned char data[3];
} eeprom_readout_t ;

int eeprom_get_serial(eeprom_readout_t *ro)
{
  unsigned char data[14];
  int try;
  /* try several times to read */
  for (try = 0; try < 5; try++) {
    debug_print("Trying to read out serial number (try nr. %d)..", try+1);

   one_wire_reset();

 //        usleep(200000);

    one_wire_write(0xCC); /* op code for skip rom 0xcc, readrom 0x33 */

    /* read memory */
    one_wire_write(0xF0); /* command read mem */
    one_wire_write(0x00); /* address 0 */
    one_wire_write(0x00); /* address 1 */


    int i;
    for (i = 0; i < sizeof(data); i++) {
      data[i] = one_wire_read();
      xil_printf("%02X ", data[i] & 0xFF);
    }
    xil_printf("\r\n");

    /* check if we read out something sensible - these are constant bytes */
    if (memcmp(data, (unsigned char []){0x8d, 0x0a, 0x29, 0x11, 0x00, 0x00}, 6) != 0) {
    	debug_print("Wrong EEPROM readout.");
      usleep(500000);
      continue;
    }

    /* crc of 13 bytes (includes inverted crc16 code) must be 0xB001 */
    if (crc16(&data[1], 13) == 0xB001) {

      memcpy(ro->data, &data[6], 3);


      debug_print("Serial number from eeprom: %02X %02X %02X .",
    		  ro->data[0], ro->data[1], ro->data[2]);
      return XST_SUCCESS;
    } else {
    	debug_print("EEPROM: inverted CRC16 not matching.");
      usleep(500000);
    }
  }

  /* failed to obtain a serial number */
  ro->data[0] = 0xAA;
  ro->data[1] = 0xBB;
  ro->data[2] = 0xCC;
  return XST_FAILURE;
}





#endif /* EEPROM_UTIL_H_ */
