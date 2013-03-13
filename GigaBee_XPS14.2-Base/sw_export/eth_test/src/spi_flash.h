/* File		: spi_flash.h
 * Company	: Trenz Electronics
 * Author	: Oleksandr Kiyenko
 */

#ifndef __SPI_FLASH_H_
#define __SPI_FLASH_H_

#include "xparameters.h"
#include "xspi.h"
#include "xil_exception.h"

#define SPI_DEVICE_ID			XPAR_QSPI_FLASH_DEVICE_ID
#define SPI_SELECT 				0x01
#define	SPI_WRITE				0x02
#define SPI_READ				0x03
#define SPI_RDSR1				0x05
#define	SPI_WREN				0x06
#define	SPI_SE					0xD8
#define SPI_RDID				0x9F

#define READ_WRITE_EXTRA_BYTES	4
#define RDSR1_BYTES				2
#define WREN_BYTES				1
#define SE_BYTES				4
#define FLASH_SR_IS_READY_MASK	0x01
#define PAGE_SIZE				256


int spi_flash_init(u32 dev_id);
int spi_flash_read(u32 addr, u32 count, u8* buf);
int spi_flash_read_l(u32 Addr, u32 ByteCount);
int spi_flash_wait(void);
void spi_flash_sector_erase(u8 sector);
void spi_flash_set_write_enable(void);
int spi_flash_write(u32 spi_addr, u32 count, u8* buf);
int spi_flash_get_id(void);

#endif
