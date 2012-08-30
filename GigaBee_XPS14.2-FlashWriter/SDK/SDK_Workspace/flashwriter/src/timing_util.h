/*
 * timing_util.h
 *
 *  Created on: Dec 15, 2011
 *      Author: Ziga Lenarcic
 */

#ifndef TIMING_UTIL_H_
#define TIMING_UTIL_H_
#include <stdio.h>
#include "xtmrctr.h"


#include "xintc.h"


static XTmrCtr TimerInstance;

// Timer functions
#if 0
	int XTmrCtr_Initialize(&TimerInstance, XPAR_AXI_TIMER_0_DEVICE_ID);
	void XTmrCtr_Start(&TimerInstance, 0);
	void XTmrCtr_Stop(&TimerInstance, 0);
	u32 XTmrCtr_GetValue(&TimerInstance, 0);
	void XTmrCtr_SetResetValue(&TimerInstance, 0,
				   u32 ResetValue);
	u32 XTmrCtr_GetCaptureValue(&TimerInstance, 0);
	int XTmrCtr_IsExpired(&TimerInstance, 0);
	void XTmrCtr_Reset(&TimerInstance, 0);
	//XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ = 62500000 Hz
#endif

const float cycles_per_usec = ((float)XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ / 1.0e6f);
const float cycles_per_ms = ((float)XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ / 1.0e3f);

int timing_init(void)
{
	return XTmrCtr_Initialize(&TimerInstance, XPAR_AXI_TIMER_0_DEVICE_ID);
}

void sleep_cycles(u32 cycles)
{
	XTmrCtr_Reset(&TimerInstance, 1);
	XTmrCtr_Start(&TimerInstance, 1);
	while (XTmrCtr_GetValue(&TimerInstance, 1) < cycles);
	XTmrCtr_Stop(&TimerInstance, 1);
}

void usleep(int usec)
{
	XTmrCtr_Reset(&TimerInstance, 1);
	XTmrCtr_Start(&TimerInstance, 1);
	u32 cycles = (u32) (cycles_per_usec * (float)usec);
	while (XTmrCtr_GetValue(&TimerInstance, 1) < cycles);
	XTmrCtr_Stop(&TimerInstance, 1);
}

void sleep(int sec)
{
	usleep(sec * 1000000);
}

void tic(void)
{
	xil_printf("--- Tic!\r\n");
	XTmrCtr_Reset(&TimerInstance, 0);
	XTmrCtr_Start(&TimerInstance, 0);
}

float toc(void)
{
	u32 timerval = XTmrCtr_GetValue(&TimerInstance, 0);
	float ms = ((float)timerval) / cycles_per_ms;
	xil_printf("--- Toc! Elapsed %d ms. timerval = %d\r\n", (u32)ms, timerval );
	return ms/ 1000.0f;
}

#endif /* TIMING_UTIL_H_ */
