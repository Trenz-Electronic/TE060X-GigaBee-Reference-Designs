/* Company		: Trenz Electronics
 * Author		: Oleksandr Kiyenko
 * Created on	: 07.01.2013
 *
 */
#include "eth_hw.h"
#include <stdio.h>

int eth_init(u32 eth_id, u8* mac, u8* ip){
	//XAxiEthernet *AxiEthernetInstancePtr = &AxiEthernetInstance;
	XAxiEthernet_Config *MacCfgPtr;
	int Status;
	int i;

	for(i = 0; i < 6; i++)	// Save MAC
		eth_mac_addr[i] = mac[i];

	for(i = 0; i < 4; i++)	// Save IP
		eth_ip_addr[i] = ip[i];

	//  Get the configuration of AxiEthernet hardware.
	MacCfgPtr = XAxiEthernet_LookupConfig(eth_id);
	XLlFifo_Initialize(&FifoInstance, MacCfgPtr->AxiDevBaseAddress);
	Status = XAxiEthernet_CfgInitialize(&AxiEthernetInstance, MacCfgPtr, MacCfgPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	Status = XAxiEthernet_SetMacAddress(&AxiEthernetInstance,mac);
	if (Status != XST_SUCCESS) {
		xil_printf("Error setting MAC address");
		return XST_FAILURE;
	}
	Status = XAxiEthernet_SetOptions(&AxiEthernetInstance, XAE_RECEIVER_ENABLE_OPTION | XAE_TRANSMITTER_ENABLE_OPTION);
	if (Status != XST_SUCCESS) {
		xil_printf("Error setting options");
		return XST_FAILURE;
	}
	XAxiEthernet_Start(&AxiEthernetInstance);
	return XST_SUCCESS;
}


u16 eth_get_link_speed(void)
{
  u16 val;
  XAxiEthernet_PhyRead(&AxiEthernetInstance,  0x7, 0x05, &val);
  if ( val & 0x8000 ) { /* bit 15 */
    return 1000;
  } else if ( val & 0x0380) { /* bit 7 = 100half, 8 = 100full, bit 9 = 100baset4 */
    return 100;
  } else if ( val & 0x0060) { /* bit 5 = 10 half, bit 6 = 10 full */
    return 10;
  }
  return 0;
}

int eth_stop(void)
{
	XAxiEthernet_Stop(&AxiEthernetInstance);
	return XST_SUCCESS;
}

int eth_receive_packet(u8 *packet){
	u32 RxFrameLength;
	if(!XLlFifo_IsRxEmpty(&FifoInstance)){
		RxFrameLength = XLlFifo_RxGetLen(&FifoInstance);
		XLlFifo_Read(&FifoInstance, packet, RxFrameLength);
		return RxFrameLength;
	}
	else
		return 0;
}

int eth_send_packet(u8 *packet, u32 len){
	u32 FifoFreeBytes;
	if(len == 0)
		return 0;

	do {
		FifoFreeBytes = XLlFifo_TxVacancy(&FifoInstance);
	} while (FifoFreeBytes < len);
	XLlFifo_Write(&FifoInstance, packet, len);
	XLlFifo_TxSetLen(&FifoInstance, len);
	return 0;
}

void eth_update_link_speed(u16 speed){
	XAxiEthernet_SetOperatingSpeed(&AxiEthernetInstance, speed);
}

