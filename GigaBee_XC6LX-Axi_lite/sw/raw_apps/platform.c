/*
 * Copyright (c) 2007 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include "arch/cc.h"

#include "platform.h"
#include "platform_config.h"
#include "platform_fs.h"

#include "xenv_standalone.h"
#include "xparameters.h"
#include "xintc.h"
#include "xil_exception.h"

#ifdef __MICROBLAZE__
#include "mb_interface.h"
#include "xtmrctr_l.h"
#elif __PPC__
#include "xexception_l.h"
#include "xtime_l.h"
#endif

#ifdef PLATFORM_STDOUT_IS_16550
#include "xuartns550_l.h"
#endif

#include "lwip/tcp.h"

void
timer_callback()
{
        /* we need to call tcp_fasttmr & tcp_slowtmr at intervals specified by lwIP.
         * It is not important that the timing is absoluetly accurate.
         */
	static int odd = 1;
	tcp_fasttmr();

	odd = !odd;
	if (odd)
		tcp_slowtmr();
}

#ifdef __MICROBLAZE__
void
xadapter_timer_handler(void *p)
{
	//unsigned *timer_base = (unsigned *)PLATFORM_TIMER_BASEADDR;
	//unsigned tcsr = 0;

	timer_callback();

        /* Load timer, clear interrupt bit */
        XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0, XTC_CSR_INT_OCCURED_MASK | XTC_CSR_LOAD_MASK);
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0,
			XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_ENABLE_INT_MASK
			| XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK);

	XIntc_AckIntr(XPAR_INTC_0_BASEADDR, PLATFORM_TIMER_INTERRUPT_MASK);
}

#define MHZ (66)
#define TIMER_TLR (25000000*((float)MHZ/100))

void
platform_setup_timer()
{
	/* set the number of cycles the timer counts before interrupting */
	/* 100 Mhz clock => .01us for 1 clk tick. For 100ms, 10000000 clk ticks need to elapse  */
	XTmrCtr_SetLoadReg(PLATFORM_TIMER_BASEADDR, 0, TIMER_TLR);

	/* reset the timers, and clear interrupts */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0, XTC_CSR_INT_OCCURED_MASK | XTC_CSR_LOAD_MASK );

	/* start the timers */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0,
			XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_ENABLE_INT_MASK
			| XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK);

	/* Register Timer handler */
	XIntc_RegisterHandler(XPAR_INTC_0_BASEADDR,
			PLATFORM_TIMER_INTERRUPT_INTR,
			(XInterruptHandler)xadapter_timer_handler,
			0);

	XIntc_EnableIntr(XPAR_INTC_0_BASEADDR, PLATFORM_TIMER_INTERRUPT_MASK);
}
#else
#define MHZ 400
#define PIT_INTERVAL (250*MHZ*1000)
void
xadapter_timer_handler(void *p)
{
	timer_callback();

	XTime_TSRClearStatusBits(XREG_TSR_CLEAR_ALL);
}

void
platform_setup_timer()
{
#ifdef XPAR_CPU_PPC440_CORE_CLOCK_FREQ_HZ
        XExc_RegisterHandler(XEXC_ID_DEC_INT, (XExceptionHandler)xadapter_timer_handler, NULL);

        /* Set DEC to interrupt every 250 mseconds */
        XTime_DECSetInterval(PIT_INTERVAL);
        XTime_TSRClearStatusBits(XREG_TSR_CLEAR_ALL);
        XTime_DECEnableAutoReload();
#else
	XExc_RegisterHandler(XEXC_ID_PIT_INT, (XExceptionHandler)xadapter_timer_handler, NULL);

	/* Set PIT to interrupt every 250 mseconds */
	XTime_PITSetInterval(PIT_INTERVAL);
	XTime_TSRClearStatusBits(XREG_TSR_CLEAR_ALL);
	XTime_PITEnableAutoReload();
	XTime_PITEnableInterrupt();
#endif
}
#endif

void platform_enable_interrupts()
{
	/*
	 * Enable non-critical exceptions.
	 */
	Xil_ExceptionEnable();
}

static XIntc intc;

void platform_setup_interrupts()
{
	XIntc *intcp;
	intcp = &intc;

	XIntc_Initialize(intcp, XPAR_INTC_0_DEVICE_ID);
	XIntc_Start(intcp, XIN_REAL_MODE);


	platform_setup_timer();

	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler) XIntc_InterruptHandler,
				intcp);


#ifdef XPAR_ETHERNET_MAC_IP2INTC_IRPT_MASK
	/* Enable timer and EMAC interrupts in the interrupt controller */
	XIntc_EnableIntr(XPAR_XPS_INTC_0_BASEADDR,
#ifdef __MICROBLAZE__
			PLATFORM_TIMER_INTERRUPT_MASK |
#endif
		        XPAR_ETHERNET_MAC_IP2INTC_IRPT_MASK);
#endif



#ifdef XPAR_INTC_0_LLTEMAC_0_VEC_ID
#ifdef __MICROBLAZE__
	XIntc_Enable(intcp, PLATFORM_TIMER_INTERRUPT_INTR);
#endif
	XIntc_Enable(intcp, XPAR_INTC_0_LLTEMAC_0_VEC_ID);
#endif


#ifdef XPAR_INTC_0_AXIETHERNET_0_VEC_ID
	XIntc_Enable(intcp, PLATFORM_TIMER_INTERRUPT_INTR);
	XIntc_Enable(intcp, XPAR_INTC_0_AXIETHERNET_0_VEC_ID);
#endif


#ifdef XPAR_INTC_0_EMACLITE_0_VEC_ID
#ifdef __MICROBLAZE__
	XIntc_Enable(intcp, PLATFORM_TIMER_INTERRUPT_INTR);
#endif
	XIntc_Enable(intcp, XPAR_INTC_0_EMACLITE_0_VEC_ID);
#endif




}

void
enable_caches()
{
#ifdef __PPC__
    XCache_EnableICache(CACHEABLE_REGION_MASK);
    XCache_EnableDCache(CACHEABLE_REGION_MASK);
#elif __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    microblaze_invalidate_icache();
    microblaze_enable_icache();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    microblaze_invalidate_dcache();
    microblaze_enable_dcache();
#endif
#endif
}

void
disable_caches()
{
#ifdef __PPC__
    XCache_DisableDCache();
    XCache_DisableICache();
#elif __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
#if !XPAR_MICROBLAZE_DCACHE_USE_WRITEBACK
    microblaze_invalidate_dcache();
#endif
    microblaze_disable_dcache();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    microblaze_invalidate_icache();
    microblaze_disable_icache();
#endif
#endif
}

int
init_platform()
{
        enable_caches();

#ifdef PLATFORM_STDOUT_IS_16550
        /* if we have a uart 16550, then that needs to be initialized */
        XUartNs550_SetBaud(PLATFORM_STDOUT_BASEADDR, XPAR_XUARTNS550_CLOCK_HZ, PLATFORM_BAUDRATE);
        XUartNs550_mSetLineControlReg(PLATFORM_STDOUT_BASEADDR, XUN_LCR_8_DATA_BITS);
#endif

	platform_setup_interrupts();

	/* initialize file system layer */
	if (platform_init_fs() < 0)
            return -1;

        return 0;
}

void cleanup_platform()
{
        disable_caches();
}
