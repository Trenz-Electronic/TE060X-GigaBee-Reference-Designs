/* File		: spi_flash.c
 * Company	: Trenz Electronics
 * Author	: Oleksandr Kiyenko
 */
#include "spi_flash.h"
#include "xspi_l.h"

/*************************** Function Prototypes ******************************/
static XSpi Spi;
u8 ReadBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES + 4];
u8 WriteBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];
/************************** Function Definitions ******************************/
/*******************************************************************************
 * Initialize SPI Flash
 * @param	None
 * @return	Operation Status
 ******************************************************************************/
int spi_flash_init(u32 dev_id){
	int Status;
	XSpi_Config *ConfigPtr;

	ConfigPtr = XSpi_LookupConfig(dev_id);
	if (ConfigPtr == NULL)
		return XST_DEVICE_NOT_FOUND;

	Status = XSpi_CfgInitialize(&Spi, ConfigPtr, ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	Status = XSpi_SetOptions(&Spi, XSP_MASTER_OPTION |
				 XSP_MANUAL_SSELECT_OPTION);
	if(Status != XST_SUCCESS)
		return XST_FAILURE;

	Status = XSpi_SetSlaveSelect(&Spi, SPI_SELECT);
	if(Status != XST_SUCCESS)
		return XST_FAILURE;

	XSpi_Start(&Spi);
	XSpi_IntrGlobalDisable(&Spi);	// Pooled mode

	return XST_SUCCESS;
}
/*******************************************************************************
 * Read data from SPI Flash
 * @param	u32 Address
 * @param	u32 Count
 * @param	u8* Buffer
 * @return	Operation Status
 ******************************************************************************/
int spi_flash_read(u32 spi_addr, u32 count, u8* buf){
	u32 i, block_size, page_rem, buf_rem;
	u32 buf_addr = 0;

	while(buf_addr < count){
		page_rem = 0x0100 - (spi_addr & 0x00ff);
		buf_rem = count - buf_addr;
		block_size = (buf_rem > page_rem) ? page_rem : buf_rem;
		if(spi_flash_read_l(spi_addr, block_size) != XST_SUCCESS)
			return XST_FAILURE;
		for(i = 0; i < block_size; i++)
			buf[buf_addr + i] = ReadBuffer[i + READ_WRITE_EXTRA_BYTES];
		spi_addr += block_size;
		buf_addr += block_size;
	}
	return XST_SUCCESS;
}

int spi_flash_read_l(u32 Addr, u32 ByteCount){
	int Status;

	Status = spi_flash_wait();
	if(Status != XST_SUCCESS)
		return XST_FAILURE;

	WriteBuffer[0] = SPI_READ;
	WriteBuffer[1] = (u8) (Addr >> 16);
	WriteBuffer[2] = (u8) (Addr >> 8);
	WriteBuffer[3] = (u8) Addr;
	return XSpi_Transfer(&Spi, WriteBuffer, ReadBuffer,
			(ByteCount + READ_WRITE_EXTRA_BYTES));
}
/*******************************************************************************
 * Read flash status register
 * @param	Spi Driver Instance
 * @return	Status register contents
 ******************************************************************************/
int spi_flash_status(void){
	WriteBuffer[0] = SPI_RDSR1;
	XSpi_Transfer(&Spi, WriteBuffer, ReadBuffer, RDSR1_BYTES);
	return ReadBuffer[1];
}

int spi_flash_wait(void){
	WriteBuffer[0] = SPI_RDSR1;
	XSpi_Transfer(&Spi, WriteBuffer, ReadBuffer, RDSR1_BYTES);
	while((ReadBuffer[1] & FLASH_SR_IS_READY_MASK) == FLASH_SR_IS_READY_MASK) {
		XSpi_Transfer(&Spi, WriteBuffer, ReadBuffer, RDSR1_BYTES);
	}

		//if(spi_flash_status() != XST_SUCCESS)
		//	return XST_FAILURE;
		//if((ReadBuffer[1] & FLASH_SR_IS_READY_MASK) == 0)
		//	break;
	//}
	return XST_SUCCESS;
}

void spi_flash_set_write_enable(void){
	WriteBuffer[0] = SPI_WREN;
	XSpi_Transfer(&Spi, WriteBuffer, NULL, WREN_BYTES);
}

void spi_flash_sector_erase(u8 sector){
	spi_flash_wait();
	spi_flash_set_write_enable();
	WriteBuffer[0] = SPI_SE;
	WriteBuffer[1] = sector;
	WriteBuffer[2] = 0;
	WriteBuffer[3] = 0;
	XSpi_Transfer(&Spi, WriteBuffer, ReadBuffer, SE_BYTES);
	spi_flash_wait();

}


int spi_flash_write_buf(u32 spi_addr, u32 count, u8* buf){
	int i;

	//xil_printf("%d ",count);
	spi_flash_wait();
	spi_flash_set_write_enable();
	WriteBuffer[0] = SPI_WRITE;
	WriteBuffer[1] = (spi_addr >> 16) & 0xFF;
	WriteBuffer[2] = (spi_addr >>  8) & 0xFF;
	WriteBuffer[3] = spi_addr & 0xFF;
	for(i = 0; i < count; i++){
		WriteBuffer[READ_WRITE_EXTRA_BYTES + i] = buf[i];
	}
	XSpi_Transfer(&Spi, WriteBuffer, NULL, (count + READ_WRITE_EXTRA_BYTES));
	return XST_SUCCESS;
}

int spi_flash_write(u32 spi_addr, u32 count, u8* buf){
	u32 cnt = count;
	//u32 pointer = 0;
	u32 pointer = 0;
	u32 page_rem, block_size;
	u32 cur_addr = spi_addr;
	//u32 i;
	//u8 tbuf[256];
	//u32 err = 0;

	while(cnt){
		if(((cur_addr+pointer) & 0xFFFF) == 0x0000){
			//xil_printf("Erasing sector %d\r\n",((cur_addr+pointer) >> 16));
			spi_flash_sector_erase((cur_addr+pointer) >> 16);
		}
		page_rem = PAGE_SIZE - ((cur_addr+pointer) & 0xFF);
		block_size = (cnt > page_rem) ? page_rem : cnt;
		spi_flash_write_buf((cur_addr+pointer), block_size, (buf+pointer));

		/*spi_flash_read((cur_addr+pointer), block_size, tbuf);
		for(i = 0; i < block_size; i++){
			if(tbuf[i] != buf[i+pointer]){
				xil_printf(">%02x != %02x\r\n",tbuf[i],buf[i+pointer]);
				err = 1;
			}
		}
		if(err){
			break;
		}
		*/
		pointer += block_size;
		cnt -= block_size;

	}
	return XST_SUCCESS;
}

int spi_flash_get_id(void){
	u32 id;
	WriteBuffer[0] = SPI_RDID;
	XSpi_Transfer(&Spi, WriteBuffer, ReadBuffer, 4);
	id = (ReadBuffer[1] << 16) | (ReadBuffer[2] << 8) | ReadBuffer[3];
	return id;
}
