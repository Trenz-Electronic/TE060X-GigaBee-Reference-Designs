/*
 * eth_util.h
 *
 *  Created on: Dec 16, 2011
 *      Author: Ziga Lenarcic
 *
 *      Implements ethernet with DMA
 */

#ifndef ETH_UTIL_H_
#define ETH_UTIL_H_


/***************************** Include Files *********************************/
#include "xaxiethernet.h"
#include "xaxidma.h"
#include "xintc.h"
#include "xil_exception.h"


#include <string.h> // memset, memcpy

#define INTC_DEVICE_ID XPAR_MICROBLAZE_0_INTC_DEVICE_ID // device ID for interrupt controller


#ifndef QUAD_TO_IP
#define QUAD_TO_IP(a,b,c,d) (((u32)(a))         | \
							 (((u32)(b)) << 8)  | \
							 (((u32)(c)) << 16) | \
							 (((u32)(d)) << 24))
#endif


/**************************** Type Definitions ******************************/

/*
 * Define an aligned data type for an ethernet frame. This declaration is
 * specific to the GNU compiler
 */
//#define ETH_FRAME_SIZE XAE_MAX_JUMBO_FRAME_SIZE
#define ETH_FRAME_SIZE 2000

typedef unsigned char EthernetFrame[ETH_FRAME_SIZE] __attribute__ ((aligned(64)));
//typedef unsigned char EthernetFrame[576] __attribute__ ((aligned(64)));


/************************** Function Prototypes *****************************/


//int eth_init(void);
//int eth_enter_loopback(void);
//int eth_start(void);
//int eth_stop(void);


/************************** Variable Definitions ****************************/

XAxiEthernet AxiEthernetInstance; // axi_ethernet
XAxiDma AxiDmaInstance; // axi_dma
XIntc XIntcInstance; // interrupt controller

EthernetFrame TxFrame;	/* Transmit buffer */ // static?
EthernetFrame RxFrame;	/* Receive buffer */

unsigned char AxiEthernetMAC[6] = { 0x00, 0x0A, 0x35, 0x01, 0x02, 0x03 };

/********************** BD RINGS in RAM **************************/
#define RXBD_CNT	16	/* Number of RxBDs to use 128 */
#define TXBD_CNT	512	/* Number of TxBDs to use */
#define BD_ALIGNMENT	XAXIDMA_BD_MINIMUM_ALIGNMENT/* Byte alignment of BDs */
/*
 * Number of bytes to reserve for BD space for the number of BDs desired
 */
#define RXBD_SPACE_BYTES (XAxiDma_BdRingMemCalc(BD_ALIGNMENT, RXBD_CNT))
#define TXBD_SPACE_BYTES (XAxiDma_BdRingMemCalc(BD_ALIGNMENT, TXBD_CNT))
/*
 * Aligned memory segments to be used for buffer descriptors
 */
char RxBdSpace[RXBD_SPACE_BYTES] __attribute__ ((aligned(BD_ALIGNMENT)));
char TxBdSpace[TXBD_SPACE_BYTES] __attribute__ ((aligned(BD_ALIGNMENT)));


/*
 * Counters to be incremented by callbacks
 */
volatile int FramesRx;	/* Num of frames that have been received */
volatile int FramesTx;	/* Num of frames that have been sent */
volatile int DeviceErrors;/* Num of errors detected in the device */

/************************** Function Implemetations *****************************/
/*****************************************************************************/
/**
*
* This is the DMA TX callback function to be called by TX interrupt handler.
* This function handles BDs finished by hardware.
*
* @param    TxRingPtr is a pointer to TX channel of the DMA engine.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
static void TxCallBack(XAxiDma_BdRing *TxRingPtr)
{
	/*
	 * Disable the transmit related interrupts
	 */
	XAxiDma_BdRingIntDisable(TxRingPtr, XAXIDMA_IRQ_ALL_MASK);
	/*
	 * Increment the counter so that main thread knows something happened
	 */
	FramesTx++;
}

/*****************************************************************************/
/**
*
* This is the DMA TX Interrupt handler function.
*
* @param	TxRingPtr is a pointer to TX channel of the DMA engine.
*
* @return	None.
*
* @note		This Interrupt handler MUST clear pending interrupts before
*		handling them by calling the call back. Otherwise the following
*		corner case could raise some issue:
*
*		A packet got transmitted and a TX interrupt got asserted. If
*		the interrupt handler calls the callback before clearing the
*		interrupt, a new packet may get transmitted in the callback.
*		This new packet then can assert one more TX interrupt before
*		the control comes out of the callback function. Now when
*		eventually control comes out of the callback function, it will
*		never know about the second new interrupt and hence while
*		clearing the interrupts, would clear the new interrupt as well
*		and will never process it.
*		To avoid such cases, interrupts must be cleared before calling
*		the callback.
*
******************************************************************************/
static void TxIntrHandler(XAxiDma_BdRing *TxRingPtr)
{

	xil_printf("TxIntrHandler\r\n");

	u32 IrqStatus;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_BdRingGetIrq(TxRingPtr);

	/* Acknowledge pending interrupts */
	XAxiDma_BdRingAckIrq(TxRingPtr, IrqStatus);
	/*
	 * If no interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		DeviceErrors++;
		xil_printf
		("AXIDma: No interrupts asserted in TX status register");
		XAxiDma_Reset(&AxiDmaInstance);
		if(!XAxiDma_ResetIsDone(&AxiDmaInstance)) {
			xil_printf ("AxiDMA: Error: Could not reset\n");
		}
		return;
	}

	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		xil_printf("AXIDMA: TX Error interrupts\n");

		/* Reset should never fail for transmit channel
		 */
		XAxiDma_Reset(&AxiDmaInstance);
		if(!XAxiDma_ResetIsDone(&AxiDmaInstance)) {

			xil_printf ("AXIDMA: Error: Could not reset\n");
		}

		return;
	}


	/*
	 * If Transmit done interrupt is asserted, call TX call back function
	 * to handle the processed BDs and raise the according flag
	 */
	if ((IrqStatus & (XAXIDMA_IRQ_DELAY_MASK | XAXIDMA_IRQ_IOC_MASK))) {
		TxCallBack(TxRingPtr);
	}
}

/*****************************************************************************/
/**
*
* This is the DMA RX callback function to be called by RX interrupt handler.
* This function handles finished BDs by hardware, attaches new buffers to those
* BDs, and give them back to hardware to receive more incoming packets
*
* @param	RxRingPtr is a pointer to RX channel of the DMA engine.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void RxCallBack(XAxiDma_BdRing *RxRingPtr)
{
	xil_printf("RxCallback packet received?\r\n");

	/*
	 * Disable the receive related interrupts
	 */
	XAxiDma_BdRingIntDisable(RxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	/*
	 * Increment the counter so that main thread knows something
	 * happened
	 */
	FramesRx++;
}

/*****************************************************************************/
/**
*
* This is the Receive handler function for examples 1 and 2.
* It will increment a shared  counter, receive and validate the frame.
*
* @param	RxRingPtr is a pointer to the DMA ring instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void RxIntrHandler(XAxiDma_BdRing *RxRingPtr)
{
	xil_printf("RxIntrHandler\r\n");

	u32 IrqStatus;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_BdRingGetIrq(RxRingPtr);

	/* Acknowledge pending interrupts */
	XAxiDma_BdRingAckIrq(RxRingPtr, IrqStatus);

	/*
	 * If no interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		DeviceErrors++;
		xil_printf
		("AXIDma: No interrupts asserted in RX status register");
		XAxiDma_Reset(&AxiDmaInstance);
		if(!XAxiDma_ResetIsDone(&AxiDmaInstance)) {
			xil_printf ("Could not reset\n");
		}
		return;
	}

	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		xil_printf("AXIDMA: RX Error interrupts\n");

		/*
		 * Reset could fail and hang
		 * NEED a way to handle this or do not call it??
		 */
		XAxiDma_Reset(&AxiDmaInstance);

		if(!XAxiDma_ResetIsDone(&AxiDmaInstance)) {
			xil_printf ("AXIDMA: Could not reset\n");
		}
		return;
	}

	/*
	 * If Reception done interrupt is asserted, call RX call back function
	 * to handle the processed BDs and then raise the according flag.
	 */
	if ((IrqStatus & (XAXIDMA_IRQ_DELAY_MASK | XAXIDMA_IRQ_IOC_MASK))) {
		RxCallBack(RxRingPtr);
	}

}


/*****************************************************************************/
/**
*
* This is the Error handler callback function and this function increments the
* the error counter so that the main thread knows the number of errors.
*
* @param	AxiEthernet is a reference to the Axi Ethernet device instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void AxiEthernetErrorHandler(XAxiEthernet *AxiEthernet)
{

	u32 Pending = XAxiEthernet_IntPending(AxiEthernet);

	if (Pending & XAE_INT_RXRJECT_MASK) {
		xil_printf("AxiEthernet: Rx packet rejected");
	}

	if (Pending & XAE_INT_RXFIFOOVR_MASK) {
		xil_printf("AxiEthernet: Rx fifo over run");
	}

	XAxiEthernet_IntClear(AxiEthernet, Pending);

	/*
	 * Bump counter
	 */
	DeviceErrors++;

}
/*****************************************************************************/
/**
*
* This function setups the interrupt system so interrupts can occur for the
* Axi Ethernet.  This function is application-specific since the actual system
* may or may not have an interrupt controller.  The Axi Ethernet could be
* directly connected to a processor without an interrupt controller.  The user
* should modify this function to fit the application.
*
* @param	IntcInstancePtr is a pointer to the instance of the Intc
*		component.
* @param	AxiEthernetInstancePtr is a pointer to the instance of the
* 		AxiEthernet component.
* @param	DmaInstancePtr is a pointer to the instance of the AXIDMA
*		component.
* @param	AxiEthernetIntrId is the Interrupt ID and is typically
*		XPAR_<INTC_instance>_<AXIETHERNET_instance>_VEC_ID
*		value from xparameters.h.
* @param	DmaRxIntrId is the interrupt id for DMA Rx and is typically
*		taken from XPAR_<AXIETHERNET_instance>_CONNECTED_DMARX_INTR
* @param	DmaTxIntrId is the interrupt id for DMA Tx and is typically
*		taken from XPAR_<AXIETHERNET_instance>_CONNECTED_DMATX_INTR
*
* @return	-XST_SUCCESS to indicate success
*		-XST_FAILURE to indicate failure
*
* @note		None.
*
******************************************************************************/
static int AxiEthernetSetupIntrSystem(XIntc *IntcInstancePtr,
				XAxiEthernet *AxiEthernetInstancePtr,
				XAxiDma *DmaInstancePtr,
				u16 AxiEthernetIntrId,
				u16 DmaRxIntrId,
				u16 DmaTxIntrId)
{
	XAxiDma_BdRing * TxRingPtr = XAxiDma_GetTxRing(DmaInstancePtr);
	XAxiDma_BdRing * RxRingPtr = XAxiDma_GetRxRing(DmaInstancePtr);
	int Status;
	/*
	 * Initialize the interrupt controller and connect the ISR
	 */
	Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Unable to intialize the interrupt controller");
		return XST_FAILURE;
	}
	Status = XIntc_Connect(IntcInstancePtr, AxiEthernetIntrId,
						(XInterruptHandler)
						AxiEthernetErrorHandler,
						AxiEthernetInstancePtr);
	Status |= XIntc_Connect(IntcInstancePtr, DmaTxIntrId,
					(XInterruptHandler) TxIntrHandler,
					TxRingPtr);
	Status |= XIntc_Connect(IntcInstancePtr, DmaRxIntrId,
				(XInterruptHandler) RxIntrHandler,
				RxRingPtr);
	if (Status != XST_SUCCESS) {
		xil_printf("Unable to connect ISR to interrupt controller");
		return XST_FAILURE;
	}

	/*
	 * Start the interrupt controller
	 */
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		xil_printf("Error starting intc");
		return XST_FAILURE;
	}


	/*
	 * Enable interrupts from the hardware
	 */
	XIntc_Enable(IntcInstancePtr, AxiEthernetIntrId);
	XIntc_Enable(IntcInstancePtr, DmaTxIntrId);
	XIntc_Enable(IntcInstancePtr, DmaRxIntrId);

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XIntc_InterruptHandler,
			(void *)(IntcInstancePtr));

	Xil_ExceptionEnable();


	return XST_SUCCESS;
}



int eth_init(XIntc *IntcInstancePtr,
		XAxiEthernet *AxiEthernetInstancePtr,
		XAxiDma *DmaInstancePtr,
		u16 AxiEthernetDeviceId,
		u16 AxiDmaDeviceId,
		u16 AxiEthernetIntrId,
		u16 DmaRxIntrId,
		u16 DmaTxIntrId)
{
	int Status;
	int LoopbackSpeed;
	XAxiEthernet_Config *MacCfgPtr;
	XAxiDma_Config* DmaConfig;
	XAxiDma_BdRing *RxRingPtr = XAxiDma_GetRxRing(DmaInstancePtr);
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(DmaInstancePtr);
	XAxiDma_Bd BdTemplate;


	/*************************************/
	/* Setup device for first-time usage */
	/*************************************/

	/*
	 *  Get the configuration of AxiEthernet hardware.
	 */
	MacCfgPtr = XAxiEthernet_LookupConfig(AxiEthernetDeviceId);

	/*
	 * Check whether DMA is present or not
	 */
	if(MacCfgPtr->AxiDevType != XPAR_AXI_DMA) {
		xil_printf
			("Device HW not configured for SGDMA mode\r\n");
		return XST_FAILURE;
	}

	DmaConfig = XAxiDma_LookupConfig(AxiDmaDeviceId);



	/*
	 * Initialize AXIDMA engine. AXIDMA engine must be initialized before
	 * AxiEthernet. During AXIDMA engine initialization, AXIDMA hardware is
	 * reset, and since AXIDMA reset line is connected to AxiEthernet, this
	 * would ensure a reset of AxiEthernet.
	 */
	Status = XAxiDma_CfgInitialize(DmaInstancePtr, DmaConfig);
	if(Status != XST_SUCCESS) {
		xil_printf("Error initializing DMA\r\n");
		return XST_FAILURE;
	}


	/*
	 * Initialize AxiEthernet hardware.
	 */
	Status = XAxiEthernet_CfgInitialize(AxiEthernetInstancePtr, MacCfgPtr,
					MacCfgPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("Error in initialize");
		return XST_FAILURE;
	}

	/*
	 * Set the MAC address
	 */
	Status = XAxiEthernet_SetMacAddress(AxiEthernetInstancePtr,
							AxiEthernetMAC);
	if (Status != XST_SUCCESS) {
		xil_printf("Error setting MAC address");
		return XST_FAILURE;
	}

	/*
	 * Setup RxBD space.
	 *
	 * We have already defined a properly aligned area of memory to store
	 * RxBDs at the beginning of this source code file so just pass its
	 * address into the function. No MMU is being used so the physical and
	 * virtual addresses are the same.
	 *
	 * Setup a BD template for the Rx channel. This template will be
	 * copied to every RxBD. We will not have to explicitly set these
	 * again.
	 */

	/*
	 * Disable all RX interrupts before RxBD space setup
	 */
	XAxiDma_BdRingIntDisable(RxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	/*
	 * Create the RxBD ring
	 */
	Status = XAxiDma_BdRingCreate(RxRingPtr, (u32) &RxBdSpace,
				     (u32) &RxBdSpace, BD_ALIGNMENT, RXBD_CNT);
	if (Status != XST_SUCCESS) {
		xil_printf("Error setting up RxBD space");
		return XST_FAILURE;
	}

	XAxiDma_BdClear(&BdTemplate);
	Status = XAxiDma_BdRingClone(RxRingPtr, &BdTemplate);
	if (Status != XST_SUCCESS) {
		xil_printf("Error initializing RxBD space");
		return XST_FAILURE;
	}

	/*
	 * Setup TxBD space.
	 *
	 * Like RxBD space, we have already defined a properly aligned area of
	 * memory to use.
	 */

	/*
	 * Create the TxBD ring
	 */
	Status = XAxiDma_BdRingCreate(TxRingPtr, (u32) &TxBdSpace,
				(u32) &TxBdSpace, BD_ALIGNMENT, TXBD_CNT);
	if (Status != XST_SUCCESS) {
		xil_printf("Error setting up TxBD space");
		return XST_FAILURE;
	}
	/*
	 * We reuse the bd template, as the same one will work for both rx
	 * and tx.
	 */
	Status = XAxiDma_BdRingClone(TxRingPtr, &BdTemplate);
	if (Status != XST_SUCCESS) {
		xil_printf("Error initializing TxBD space");
		return XST_FAILURE;
	}

	/*
	 * Set PHY to loopback, speed depends on phy type.
	 * MII is 100 and all others are 1000.
	 */
	if (XAxiEthernet_GetPhysicalInterface(AxiEthernetInstancePtr) ==
						XAE_PHY_TYPE_MII) {
		LoopbackSpeed = 100;
	} else {
		LoopbackSpeed = 1000;
	}
	//AxiEthernetUtilEnterLoopback(AxiEthernetInstancePtr, LoopbackSpeed);
	/*
	 * Set PHY<-->MAC data clock
	 */
	Status =  XAxiEthernet_SetOperatingSpeed(AxiEthernetInstancePtr,
							(u16)LoopbackSpeed);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setting the operating speed of the MAC needs a delay.  There
	 * doesn't seem to be register to poll, so please consider this
	 * during your application design.
	 */

	sleep(2);

	/*
	 * Connect to the interrupt controller and enable interrupts
	 */
	Status = AxiEthernetSetupIntrSystem(IntcInstancePtr,
						AxiEthernetInstancePtr,
						DmaInstancePtr,
						AxiEthernetIntrId,
						DmaRxIntrId,
						DmaTxIntrId);




	return XST_SUCCESS;
}

int eth_enter_loopback(void)
{
	return XST_SUCCESS;
}

int eth_start(void)
{
	int Status;

	/*
	 * Make sure Tx and Rx are enabled
	 */
	Status = XAxiEthernet_SetOptions(&AxiEthernetInstance,
			XAE_RECEIVER_ENABLE_OPTION |
			XAE_TRANSMITTER_ENABLE_OPTION);
	if (Status != XST_SUCCESS) {
		xil_printf("Error setting options");
		return XST_FAILURE;
	}
	XAxiEthernet_Start(&AxiEthernetInstance);
	XAxiEthernet_IntEnable(&AxiEthernetInstance, XAE_INT_RECV_ERROR_MASK);

	return XST_SUCCESS;
}

int eth_stop(void)
{
	/*
	 * Disable the interrupts for the Axi Ethernet device
	 */

	/*
	 * Disconnect the interrupts for the DMA TX and RX channels
	 *
	 * interrupt ids:
	 * XPAR_INTC_0_AXIETHERNET_0_VEC_ID
	 * XPAR_AXIETHERNET_0_CONNECTED_DMARX_INTR
	 * XPAR_AXIETHERNET_0_CONNECTED_DMATX_INTR
	 */
	XIntc_Disconnect(&XIntcInstance, XPAR_AXIETHERNET_0_CONNECTED_DMATX_INTR);
	XIntc_Disconnect(&XIntcInstance, XPAR_AXIETHERNET_0_CONNECTED_DMARX_INTR);

	/*
	 * Disconnect and disable the interrupt for the AxiEthernet device
	 */
	XIntc_Disconnect(&XIntcInstance, XPAR_INTC_0_AXIETHERNET_0_VEC_ID);
	/*
	 * Stop the device
	 */
	XAxiEthernet_Stop(&AxiEthernetInstance);
	return XST_SUCCESS;
}

#endif /* ETH_UTIL_H_ */
