/******************************************************************************
*
* (c) Copyright 2010 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information of Xilinx, Inc.
* and is protected under U.S. and international copyright and other
* intellectual property laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
******************************************************************************/
/*****************************************************************************/
/**
 *
 * @file example_intr.c
 *
 * This file demonstrates how to use the xaxidma driver on the Xilinx AXI 
 * DMA core (AXIDMA) to transfer packets. The interrupt feature of the AXIDMA
 * core is utilized in this code.
 *
 * We show how to do multiple packets transfers, as well as how to do multiple
 * BDs per packet transfers.
 *
 * This code assumes a loopback hardware widget is connected to the AXI DMA
 * core for data packet loopback. The loopback widget core can be downloaded
 * from Answer Record #38509:
 *     ftp://ftp.xilinx.com/pub/applications/misc/ar38509.zip
 *
 * To see the debug print, you need a Uart16550 or uartlite in your system,
 * and please set "-DDEBUG" in your compiler options. You need to rebuild your
 * software executable.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -------------------------------------------------------
 * 1.00a jz   05/18/10 First release
 * 2.00a jz   08/10/10 Second release, added in xaxidma_g.c, xaxidma_sinit.c,
 *                     updated tcl file, added xaxidma_porting_guide.h, removed
 *                     workaround for endianness
 * </pre>
 *
 * ***************************************************************************
 */

#include "xaxidma.h"
#include "xparameters.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xdebug.h"

#ifdef XPAR_UARTNS550_0_BASEADDR
#include "xuartns550_l.h"       /* to use uartns550 */
#endif

#ifndef DEBUG
extern int xil_printf(const char *format, ...);
#endif

/******************** Constant Definitions **********************************/
/*
 * Device hardware build related constants.
 */
#define DMA_BASE_ADDR       XPAR_AXIDMA_0_BASEADDR
#define DMA_DEV_ID          XPAR_AXIDMA_0_DEVICE_ID

#ifdef XPAR_V6DDR_0_S_AXI_BASEADDR
#define DDR_BASE_ADDR      XPAR_V6DDR_0_S_AXI_BASEADDR
#endif

#ifdef XPAR_S6DDR_0_S0_AXI_BASEADDR
#define DDR_BASE_ADDR      XPAR_S6DDR_0_S0_AXI_BASEADDR
#endif

#define MEM_BASE_ADDR       (DDR_BASE_ADDR + 0x1000000)
#define RX_INTR_ID			XPAR_INTC_0_AXIDMA_0_S2MM_INTROUT_VEC_ID	
#define TX_INTR_ID			XPAR_INTC_0_AXIDMA_0_MM2S_INTROUT_VEC_ID	
#define RX_BD_SPACE_BASE    (MEM_BASE_ADDR)
#define RX_BD_SPACE_HIGH    (MEM_BASE_ADDR + 0x0000FFFF)
#define TX_BD_SPACE_BASE    (MEM_BASE_ADDR + 0x00010000)
#define TX_BD_SPACE_HIGH    (MEM_BASE_ADDR + 0x0001FFFF)
#define TX_BUFFER_BASE      (MEM_BASE_ADDR + 0x01000000)
#define RX_BUFFER_BASE      (MEM_BASE_ADDR + 0x03000000)
#define RX_BUFFER_HIGH      (MEM_BASE_ADDR + 0x04FFFFFF)

#define INTC_DEVICE_ID     	XPAR_INTC_0_DEVICE_ID 

/* Timeout loop counter for reset
 */
#define RESET_TIMEOUT_COUNTER	10000

/*
 * Buffer and Buffer Descriptor related constant definition
 */
#define MAX_PKT_LEN      0x100

/*
 * Number of BDs in the transfer example
 * We show how to submit multiple BDs for one transmit.
 * The receive side gets one completion per transfer.
 */
#define NUMBER_OF_BDS_PER_PKT      12
#define NUMBER_OF_PKTS_TO_TRANSFER 11 
#define NUMBER_OF_BDS_TO_TRANSFER	(NUMBER_OF_PKTS_TO_TRANSFER * \
						 NUMBER_OF_BDS_PER_PKT)	

/* The interrupt coalescing threshold and delay timer threshold
 * Valid range is 1 to 255
 *
 * We set the coalescing threshold to be the total number of packets.
 * The receive side will only get one completion interrupt for this example.
 */ 
#define COALESCING_COUNT    NUMBER_OF_PKTS_TO_TRANSFER	
#define DELAY_TIMER_COUNT	100

/*
 * Device instance definitions
 */
XAxiDma AxiDma;
XIntc Intc;

/*
 * Flags interrupt handlers use to notify the application context the events.
 */
volatile int TxDone;
volatile int RxDone;
volatile int Error;

/*
 * Buffer for transmit packet. Must be 32-bit aligned to be used by DMA.
 */
u32 *Packet = (u32 *) TX_BUFFER_BASE;

#ifdef XPAR_UARTNS550_0_BASEADDR
/*****************************************************************************/
/*
*
* Uart16550 setup routine, need to set baudrate to 9600 and data bits to 8
*
*
******************************************************************************/
static void Uart550_Setup(void) {

	XUartNs550_SetBaud(XPAR_UARTNS550_0_BASEADDR,
			XPAR_XUARTNS550_CLOCK_HZ, 9600);

	XUartNs550_SetLineControlReg(XPAR_UARTNS550_0_BASEADDR,
			XUN_LCR_8_DATA_BITS);
}
#endif

/*****************************************************************************/
/*
*
* This function checks data buffer after the DMA transfer is finished.
*
* We use the static tx/rx buffers.
*
* @param Length is the length to check
* @param StartValue is the starting value of the first byte
*
* @return   XST_SUCCESS if validation is successful, XST_FAILURE otherwise.
*
* @note     None.
*
******************************************************************************/
static int CheckData(int Length, u8 StartValue)
{
	u8 *RxPacket;
	int i = 0;
	u8 Value;

	RxPacket = (u8 *) RX_BUFFER_BASE;
	Value = StartValue;

	for(i = 0; i < Length; i++) {
		if (RxPacket[i] != Value) {
			xdbg_printf(XDBG_DEBUG_ERROR,
			    "Data error %d: %x/%x\r\n",
			    i, RxPacket[i], Value);
		
			return XST_FAILURE;
		}
		Value = (Value + 1) & 0xFF;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/*
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
static void TxCallBack(XAxiDma_BdRing * TxRingPtr)
{
	int BdCount;
	u32 BdSts;
	XAxiDma_Bd *BdPtr;
	XAxiDma_Bd *BdCurPtr;
	int Status;
	int i;

	/* Get all processed BDs from hardware */
	BdCount = XAxiDma_BdRingFromHw(TxRingPtr, XAXIDMA_ALL_BDS, &BdPtr);

	/* Handle the BDs */
	BdCurPtr = BdPtr;
	for (i = 0; i < BdCount; i++) {

		/* Check the status in each BD */
		/* If error happens, the DMA engine will be halted after this BD,
		 * processing stops.
		 */ 
		BdSts = XAxiDma_BdGetSts(BdCurPtr);
		if ((BdSts & XAXIDMA_BD_STS_ALL_ERR_MASK) ||
		    (!(BdSts & XAXIDMA_BD_STS_COMPLETE_MASK))) {

			xdbg_printf(XDBG_DEBUG_ERROR, "Error status %x\r\n",
				(unsigned int)BdSts);
			Error = 1;
			break;
		}
	
		/* Here we don't need to do anything. But if a RTOS is being
		 * used, we may need to free the packet buffer attached to
		 * the processed BD
		 */

		/* Find the next processed BD */
		BdCurPtr = XAxiDma_BdRingNext(TxRingPtr, BdCurPtr);
	}

	/* Free all processed BDs for future transmission */
	Status = XAxiDma_BdRingFree(TxRingPtr, BdCount, BdPtr);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Error free BD %x\r\n", Status);
		Error = 1;
	}

	if(!Error) {
		TxDone += BdCount;
	}
	else {
		xdbg_printf(XDBG_DEBUG_ERROR, "Transfer tx error\r\n");
	}
}

/*****************************************************************************/
/*
*
* This is the DMA TX Interrupt handler function.
*
* It gets the interrupt status from the hardware, acknowledges it, and if any
* error happens, it resets the hardware. Otherwise, if a completion interrupt
* presents, then it calls the callback function.
*
* @param    Callback is a pointer to TX channel of the DMA engine.
*
* @return   None.
*
******************************************************************************/
static void TxIntrHandler(void *Callback)
{
	XAxiDma_BdRing *TxRingPtr = (XAxiDma_BdRing *) Callback;
	u32 IrqStatus;
	int TimeOut;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_BdRingGetIrq(TxRingPtr);

	/* Acknowledge pending interrupts */
	XAxiDma_BdRingAckIrq(TxRingPtr, IrqStatus);

	/* If no interrupt is asserted, we do not do anything 
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Error, no irq %x\n",
			(unsigned int)IrqStatus);

		return;
	}

	/* If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Error, err irq %x\n",
			(unsigned int)IrqStatus);

		XAxiDma_BdRingDumpRegs(TxRingPtr);

		Error = 1;

		/* Reset should never fail for transmit channel
		 */
		XAxiDma_Reset(&AxiDma);

		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut) {
			if (XAxiDma_ResetIsDone(&AxiDma)) {
				break;
			}

			TimeOut -= 1;
		}

		if (!TimeOut) {

			xdbg_printf(XDBG_DEBUG_ERROR, "Reset failed for transmit "
				"channel upon error irq\r\n");
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
/*
*
* This is the DMA RX callback function called by the RX interrupt handler.
* This function handles finished BDs by hardware, attaches new buffers to those
* BDs, and give them back to hardware to receive more incoming packets
*
* @param    RxRingPtr is a pointer to RX channel of the DMA engine.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
static void RxCallBack(XAxiDma_BdRing * RxRingPtr)
{
	int BdCount;
	XAxiDma_Bd *BdPtr;
	XAxiDma_Bd *BdCurPtr;
	u32 BdSts;
	int i;

	/* Get finished BDs from hardware */
	BdCount = XAxiDma_BdRingFromHw(RxRingPtr, XAXIDMA_ALL_BDS, &BdPtr);

	BdCurPtr = BdPtr;
	for (i = 0; i < BdCount; i++) {

		/* Check the flags set by the hardware for status */
		/* If error happens, processing stops, because the DMA engine is
		 * halted after this BD.
		 */
		BdSts = XAxiDma_BdGetSts(BdCurPtr);
		if ((BdSts & XAXIDMA_BD_STS_ALL_ERR_MASK) ||
		    (!(BdSts & XAXIDMA_BD_STS_COMPLETE_MASK))) {

			xdbg_printf(XDBG_DEBUG_ERROR, "Err Rx status %x\r\n",
				(unsigned int)BdSts);
			Error = 1;
			break;
		}

		/* Find the next processed BD */
		BdCurPtr = XAxiDma_BdRingNext(RxRingPtr, BdCurPtr);
		RxDone += 1;
	}

}

/*****************************************************************************/
/*
*
* This is the DMA RX interrupt handler function
*
* It gets the interrupt status from the hardware, acknowledges it, and if any
* error happens, it resets the hardware. Otherwise, if a completion interrupt
* presents, then it calls the callback function.
*
* @param    Callback is a pointer to RX channel of the DMA engine.
*
* @return   None.
*
******************************************************************************/
static void RxIntrHandler(void *Callback)
{
	XAxiDma_BdRing *RxRingPtr = (XAxiDma_BdRing *) Callback;
	u32 IrqStatus;
	int TimeOut;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_BdRingGetIrq(RxRingPtr);

	/* Acknowledge pending interrupts */
	XAxiDma_BdRingAckIrq(RxRingPtr, IrqStatus);

	/* If no interrupt is asserted, we do not do anything
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		xdbg_printf(XDBG_DEBUG_ERROR, "Rx no irq %x?\r\n",
			(unsigned int)IrqStatus);

		return;
	}

	/* If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Error rx err irq %x\r\n",
			(unsigned int)IrqStatus);

		XAxiDma_BdRingDumpRegs(RxRingPtr);

		Error = 1;

		/* Reset could fail and hang
		 * NEED a way to handle this or do not call it??
		 */
		XAxiDma_Reset(&AxiDma);

		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut) {
			if(XAxiDma_ResetIsDone(&AxiDma)) {
				break;
			}

			TimeOut -= 1;
		}

		if (!TimeOut) {

			xdbg_printf(XDBG_DEBUG_ERROR, "Reset failed for transmit "
				"channel upon error irq\r\n");
		}

		return;
	}

	/*
	 * If completion interrupt is asserted, call RX call back function
	 * to handle the processed BDs and then raise the according flag.
	 */
	if ((IrqStatus & (XAXIDMA_IRQ_DELAY_MASK | XAXIDMA_IRQ_IOC_MASK))) {

		RxCallBack(RxRingPtr);
	}
}

/*****************************************************************************/
/*
*
* This function setups the interrupt system so interrupts can occur for the
* DMA.  This function assumes INTC component exists in the hardware system.
*
* @param    IntcInstancePtr is a pointer to the instance of the INTC component.
* @param    AxiDmaPtr is a pointer to the instance of the DMA engine
* @param    TxIntrId is the TX channel Interrupt ID.
* @param    RxIntrId is the RX channel Interrupt ID.
*
* @return   XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note     None.
*
******************************************************************************/
static int SetupIntrSystem(XIntc * IntcInstancePtr,
			   XAxiDma * AxiDmaPtr, u16 TxIntrId, u16 RxIntrId)
{
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(AxiDmaPtr);
	XAxiDma_BdRing *RxRingPtr = XAxiDma_GetRxRing(AxiDmaPtr);
	int Status;

	/* Initialize the interrupt controller and connect the ISRs */
	Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed init intc\r\n");
		return XST_FAILURE;
	}

	Status = XIntc_Connect(IntcInstancePtr, TxIntrId,
			       (XInterruptHandler) TxIntrHandler, TxRingPtr);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed tx connect intc\r\n");
		return XST_FAILURE;
	}

	Status = XIntc_Connect(IntcInstancePtr, RxIntrId,
			       (XInterruptHandler) RxIntrHandler, RxRingPtr);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed rx connect intc\r\n");
		return XST_FAILURE;
	}

	/* Start the interrupt controller */
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed to start intc\r\n");
		return XST_FAILURE;
	}

	/* Enable interrupts from the hardware */
	XIntc_Enable(IntcInstancePtr, TxIntrId);
	XIntc_Enable(IntcInstancePtr, RxIntrId);

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, 
			(Xil_ExceptionHandler)XIntc_InterruptHandler,
			(void *)IntcInstancePtr);

	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function disables the interrupts for DMA engine.
*
* @param    IntcInstancePtr is the pointer to the INTC component instance
* @param    TxIntrId is interrupt ID associated w/ DMA TX channel
* @param    RxIntrId is interrupt ID associated w/ DMA RX channel
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
static void DisableIntrSystem(XIntc * IntcInstancePtr,
			      u16 TxIntrId, u16 RxIntrId)
{

	/* Disconnect the interrupts for the DMA TX and RX channels */
	XIntc_Disconnect(IntcInstancePtr, TxIntrId);
	XIntc_Disconnect(IntcInstancePtr, RxIntrId);
}

/*****************************************************************************/
/*
*
* This function sets up RX channel of the DMA engine to be ready for packet
* reception
*
* @param    AxiDmaInstPtr is the pointer to the instance of the DMA engine.
*
* @return   XST_SUCCESS if the setup is successful, XST_FAILURE otherwise.
*
* @note     None.
*
******************************************************************************/
static int RxSetup(XAxiDma * AxiDmaInstPtr)
{
	XAxiDma_BdRing *RxRingPtr;
	int Status;
	XAxiDma_Bd BdTemplate;
	XAxiDma_Bd *BdPtr;
	XAxiDma_Bd *BdCurPtr;
	int BdCount;
	int FreeBdCount;
	u32 RxBufferPtr;
	int i;

	RxRingPtr = XAxiDma_GetRxRing(&AxiDma);

	/* Disable all RX interrupts before RxBD space setup */
	XAxiDma_BdRingIntDisable(RxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	/* Setup Rx BD space */
	BdCount = XAxiDma_BdRingCntCalc(XAXIDMA_BD_MINIMUM_ALIGNMENT,
					RX_BD_SPACE_HIGH - RX_BD_SPACE_BASE + 1);

	Status = XAxiDma_BdRingCreate(RxRingPtr, RX_BD_SPACE_BASE,
				     RX_BD_SPACE_BASE,
				     XAXIDMA_BD_MINIMUM_ALIGNMENT, BdCount);
	if (Status != XST_SUCCESS) {
		xdbg_printf(XDBG_DEBUG_ERROR,
		    "Rx bd create failed with %d\r\n", Status);
		return XST_FAILURE;
	}

	/*
	 * Setup a BD template for the Rx channel. Then copy it to every RX BD.
	 */
	XAxiDma_BdClear(&BdTemplate);
	Status = XAxiDma_BdRingClone(RxRingPtr, &BdTemplate);
	if (Status != XST_SUCCESS) {
		xdbg_printf(XDBG_DEBUG_ERROR,
		    "Rx bd clone failed with %d\r\n", Status);
		return XST_FAILURE;
	}

	/* Attach buffers to RxBD ring so we are ready to receive packets */
	FreeBdCount = XAxiDma_BdRingGetFreeCnt(RxRingPtr);

	Status = XAxiDma_BdRingAlloc(RxRingPtr, FreeBdCount, &BdPtr);
	if (Status != XST_SUCCESS) {
		xdbg_printf(XDBG_DEBUG_ERROR,
		    "Rx bd alloc failed with %d\r\n", Status);
		return XST_FAILURE;
	}

	BdCurPtr = BdPtr;
	RxBufferPtr = RX_BUFFER_BASE;

	for (i = 0; i < FreeBdCount; i++) {

		Status = XAxiDma_BdSetBufAddr(BdCurPtr, RxBufferPtr);
		if (Status != XST_SUCCESS) {
			xdbg_printf(XDBG_DEBUG_ERROR,
			    "Rx set buffer addr %x on BD %x failed %d\r\n",
			    (unsigned int)RxBufferPtr, (unsigned int)BdCurPtr, Status);

			return XST_FAILURE;
		}

		Status = XAxiDma_BdSetLength(BdCurPtr, MAX_PKT_LEN);
		if (Status != XST_SUCCESS) {
			xdbg_printf(XDBG_DEBUG_ERROR,
			    "Rx set length %d on BD %x failed %d\r\n",
			    MAX_PKT_LEN, (unsigned int)BdCurPtr, Status);

			return XST_FAILURE;
		}

		/* Receive BDs do not need to set anything for the control
		 * The hardware will set the SOF/EOF bits per stream status 
		 */
		XAxiDma_BdSetCtrl(BdCurPtr, 0);

		XAxiDma_BdSetId(BdCurPtr, RxBufferPtr);

		RxBufferPtr += MAX_PKT_LEN;
		BdCurPtr = XAxiDma_BdRingNext(RxRingPtr, BdCurPtr);
	}

	/* Set the coalescing threshold, so only one receive interrupt
	 * occurs for this example
	 *
	 * If you would like to have multiple interrupts to happen, change
	 * the COALESCING_COUNT to be a smaller value
	 */
	Status = XAxiDma_BdRingSetCoalesce(RxRingPtr, COALESCING_COUNT,
			DELAY_TIMER_COUNT);
	if (Status != XST_SUCCESS) {
		xdbg_printf(XDBG_DEBUG_ERROR,
		    "Rx set coalesce failed with %d\r\n", Status);
		return XST_FAILURE;
	}

	Status = XAxiDma_BdRingToHw(RxRingPtr, FreeBdCount, BdPtr);
	if (Status != XST_SUCCESS) {
		xdbg_printf(XDBG_DEBUG_ERROR,
		    "Rx ToHw failed with %d\r\n", Status);
		return XST_FAILURE;
	}

	/* Enable all RX interrupts */
	XAxiDma_BdRingIntEnable(RxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	/* Start RX DMA channel */
	Status = XAxiDma_BdRingStart(RxRingPtr);
	if (Status != XST_SUCCESS) {
		xdbg_printf(XDBG_DEBUG_ERROR,
		    "Rx start BD ring failed with %d\r\n", Status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/*
*
* This function sets up the TX channel of a DMA engine to be ready for packet
* transmission.
*
* @param    AxiDmaInstPtr is the pointer to the instance of the DMA engine.
*
* @return   XST_SUCCESS if the setup is successful, XST_FAILURE otherwise.
*
* @note     None.
*
******************************************************************************/
static int TxSetup(XAxiDma * AxiDmaInstPtr)
{
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(&AxiDma);
	XAxiDma_Bd BdTemplate;
	int Status;
	u32 BdCount;

	/* Disable all TX interrupts before TxBD space setup */
	XAxiDma_BdRingIntDisable(TxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	/* Setup TxBD space  */
	BdCount = XAxiDma_BdRingCntCalc(XAXIDMA_BD_MINIMUM_ALIGNMENT,
					(u32)TX_BD_SPACE_HIGH - (u32)TX_BD_SPACE_BASE + 1);

	Status = XAxiDma_BdRingCreate(TxRingPtr, TX_BD_SPACE_BASE,
				     TX_BD_SPACE_BASE,
				     XAXIDMA_BD_MINIMUM_ALIGNMENT, BdCount);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed create BD ring\r\n");
		return XST_FAILURE;
	}

	/*
	 * Like the RxBD space, we create a template and set all BDs to be the
	 * same as the template. The sender has to set up the BDs as needed.
	 */
	XAxiDma_BdClear(&BdTemplate);
	Status = XAxiDma_BdRingClone(TxRingPtr, &BdTemplate);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed clone BDs\r\n");
		return XST_FAILURE;
	}

	/* Set the coalescing threshold, so only one transmit interrupt
	 * occurs for this example
	 *
	 * If you would like to have multiple interrupts to happen, change
	 * the COALESCING_COUNT to be a smaller value
	 */
	Status = XAxiDma_BdRingSetCoalesce(TxRingPtr, COALESCING_COUNT,
			DELAY_TIMER_COUNT);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed set coalescing %d/%d\r\n",
			COALESCING_COUNT, DELAY_TIMER_COUNT);
		return XST_FAILURE;
	}

	/* Enable all TX interrupts */
	XAxiDma_BdRingIntEnable(TxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	/* Start the TX channel */
	Status = XAxiDma_BdRingStart(TxRingPtr);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed bd start\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/*
*
* This function non-blockingly transmits all packets through the DMA engine.
*
* @param    AxiDmaInstPtr points to the DMA engine instance
*
* @return
* - XST_SUCCESS if the DMA accepts all the packets successfully,
* - XST_FAILURE if error occurs
*
******************************************************************************/
static int SendPacket(XAxiDma * AxiDmaInstPtr)
{
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(AxiDmaInstPtr);
	u8 *TxPacket;
	u8 Value;
	XAxiDma_Bd *BdPtr, *BdCurPtr;
	int Status;
	int i, j;
	u32 BufferAddr;

	/* Each packet is limited to XAXIDMA_MAX_TRANSFER_LEN
	 *
	 * This will not be the case if hardware has store and forward built in
	 */ 
	if (MAX_PKT_LEN * NUMBER_OF_BDS_PER_PKT > XAXIDMA_MAX_TRANSFER_LEN) {

		xdbg_printf(XDBG_DEBUG_ERROR,
		    "Invalid total per packet transfer length for the "
		    "packet %d/%d\r\n",
		    MAX_PKT_LEN * NUMBER_OF_BDS_PER_PKT,
		    XAXIDMA_MAX_TRANSFER_LEN);

		return XST_INVALID_PARAM;
	}

	TxPacket = (u8 *) Packet;

	Value = 0xC;

	for(i = 0; i < MAX_PKT_LEN * NUMBER_OF_BDS_TO_TRANSFER; i ++) {
		TxPacket[i] = Value;

		Value = (Value + 1) & 0xFF;
	}
		
	Status = XAxiDma_BdRingAlloc(TxRingPtr, NUMBER_OF_BDS_TO_TRANSFER, &BdPtr);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed bd alloc\r\n");
		return XST_FAILURE;
	}

	BufferAddr = (u32) Packet;
	BdCurPtr = BdPtr;

	/* Set up the BD using the information of the packet to transmit
	 *
	 * Each transfer has NUMBER_OF_BDS_PER_PKT BDs
	 */
	for(i = 0; i < NUMBER_OF_PKTS_TO_TRANSFER; i++) {

		for(j = 0; j < NUMBER_OF_BDS_PER_PKT; j++) {
			u32 CrBits = 0;

			Status = XAxiDma_BdSetBufAddr(BdCurPtr, BufferAddr);
			if (Status != XST_SUCCESS) {
				xdbg_printf(XDBG_DEBUG_ERROR,
				    "Tx set buffer addr %x on BD %x failed %d\r\n",
				    (unsigned int)BufferAddr, (unsigned int)BdCurPtr, Status);

				return XST_FAILURE;
			}

			Status = XAxiDma_BdSetLength(BdCurPtr, MAX_PKT_LEN);
			if (Status != XST_SUCCESS) {
				xdbg_printf(XDBG_DEBUG_ERROR,
				    "Tx set length %d on BD %x failed %d\r\n",
				    MAX_PKT_LEN, (unsigned int)BdCurPtr, Status);

				return XST_FAILURE;
			}

			if (j == 0) {
				/* The first BD has SOF set
				 */
				CrBits |= XAXIDMA_BD_CTRL_TXSOF_MASK;

#if (XPAR_AXIDMA_0_SG_INCLUDE_STSCNTRL_STRM == 1)
				/* The first BD has total transfer length set in
				 * the last APP word, this is for the loopback widget
				 */
				Status = XAxiDma_BdSetAppWord(BdCurPtr,
				    XAXIDMA_LAST_APPWORD,
				    MAX_PKT_LEN * NUMBER_OF_BDS_PER_PKT);

				if (Status != XST_SUCCESS) {
					xdbg_printf(XDBG_DEBUG_ERROR,
					    "Set app word failed with %d\r\n", Status);
				}
#endif
			} 

			if(j == (NUMBER_OF_BDS_PER_PKT - 1)) {
				/* The last BD should have EOF and IOC set
				 */
				CrBits |= XAXIDMA_BD_CTRL_TXEOF_MASK;
			}

			XAxiDma_BdSetCtrl(BdCurPtr, CrBits); 
			XAxiDma_BdSetId(BdCurPtr, BufferAddr);

			BufferAddr += MAX_PKT_LEN;
			BdCurPtr = XAxiDma_BdRingNext(TxRingPtr, BdCurPtr);
		}
	}

	/* Give the BD to hardware */
	Status = XAxiDma_BdRingToHw(TxRingPtr, NUMBER_OF_BDS_TO_TRANSFER, BdPtr);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed to hw, length %d\r\n",
			(int)XAxiDma_BdGetLength(BdPtr));

		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* Main function
*
* This function is the main entry of the interrupt test. It does the following:
*	. Set up the output terminal if UART16550 is in the hardware build
*	. Initialize the DMA engine
*	. Set up Tx and Rx channels
*	. Set up the interrupt system for the Tx and Rx interrupts
*	. Submit a transfer
*	. Wait for the transfer to finish
*	. Check transfer status
*	. Disable Tx and Rx interrupts
*	. Print test status and exit
*
* @return   0 if tests pass, 1 otherwise.
*
* @note     None.
*
******************************************************************************/
int main(void)
{
	int Status;
	XAxiDma_Config *Config;

	/* Initial setup for Uart16550 */
#ifdef XPAR_UARTNS550_0_BASEADDR

	Uart550_Setup();

#endif

	xil_printf("\r\n--- Entering main() --- \r\n");

	Config = XAxiDma_LookupConfig(DMA_DEV_ID);
	if (!Config) {
		xdbg_printf(XDBG_DEBUG_ERROR,
		    "No config found for %d\r\n", DMA_DEV_ID);

		return 1;
	}

	/* Initialize DMA engine */
	XAxiDma_CfgInitialize(&AxiDma, Config);

	/* Set up TX/RX channels to be ready to transmit and receive packets */
	Status = TxSetup(&AxiDma);
	
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed TX setup\r\n");
		return 1;
	}

	Status = RxSetup(&AxiDma);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed RX setup\r\n");
		return 1;
	}

	/* Set up Interrupt system  */
	Status = SetupIntrSystem(&Intc, &AxiDma, TX_INTR_ID, RX_INTR_ID);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed intr setup\r\n");
		return 1;
	}

	/* Initialize flags before start transfer test  */
	TxDone = 0;
	RxDone = 0;
	Error = 0;

	/* Send a packet */
	Status = SendPacket(&AxiDma);
	if (Status != XST_SUCCESS) {

		xdbg_printf(XDBG_DEBUG_ERROR, "Failed send packet\r\n");
		return 1;
	}

	/* Wait TX done and RX done
	 */
	while (((TxDone < NUMBER_OF_BDS_TO_TRANSFER)
		|| (RxDone < NUMBER_OF_BDS_TO_TRANSFER)) && !Error) {
		/* NOP */
	}

	if (Error) {
		xil_printf("Failed test transmit%s done, "
			"receive%s done\r\n", TxDone? "":" not", RxDone? "":" not");

		goto Done;

	}else {

		/* Test finished, check data
		 */
		Status = CheckData(MAX_PKT_LEN * NUMBER_OF_BDS_TO_TRANSFER, 0xC);
		if (Status != XST_SUCCESS) {

			xil_printf("Data check failed\r\n");

			goto Done;
		}

		xil_printf("Test passed\r\n");
	}

	/* Disable TX and RX Ring interrupts and return success */

	DisableIntrSystem(&Intc, TX_INTR_ID, RX_INTR_ID);

Done:
	xil_printf("--- Exiting main() --- \r\n");

	return 0;
}
