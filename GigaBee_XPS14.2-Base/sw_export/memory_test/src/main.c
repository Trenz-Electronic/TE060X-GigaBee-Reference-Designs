#include <stdio.h>
#include "xparameters.h"
#include "xuartns550.h"

XUartNs550 UARTInst;

u32 pattern1 = 0xA5A5A5A5;
u32 pattern2 = 0x5A5A5A5A;
volatile u32 *ddr_mem = (u32*)XPAR_MCB_DDR3_S0_AXI_BASEADDR;


int main()
{
	u32 i, percent, complete, prev_complete;
	u32 test_size = (XPAR_MCB_DDR3_S0_AXI_HIGHADDR - XPAR_MCB_DDR3_S0_AXI_BASEADDR)/sizeof(u32);
	u32 error = 0;

    XUartNs550_Initialize(&UARTInst, XPAR_RS232_UART_1_DEVICE_ID);
    XUartNs550_SetBaud(XPAR_RS232_UART_1_BASEADDR, XPAR_RS232_UART_1_CLOCK_FREQ_HZ, 115200);
    xil_printf("Memory test\n\r");
    xil_printf("Base Address : 0x%04X\r\n",XPAR_MCB_DDR3_S0_AXI_BASEADDR);
    xil_printf("High Address : 0x%04X\r\n\r\n",XPAR_MCB_DDR3_S0_AXI_HIGHADDR);
    //-------------------------------------------------------------------------
    xil_printf("Filling memory with pattern1...\n\r");
    percent = test_size / 100;
    prev_complete = 1;
    for(i = 0; i < test_size; i++){
    	ddr_mem[i] = pattern1;
    	complete = i / percent;
    	if(complete != prev_complete){
    		xil_printf("%d%%\r",complete);
    		prev_complete = complete;
    	}
    }

    xil_printf("Checking...\n\r");
    prev_complete = 1;
    for(i = 0; i < test_size; i++){
    	if(ddr_mem[i] != pattern1){
    		xil_printf("Test Failed @%04X %04X != %04X",i,ddr_mem[i],pattern1);
    		error = 1;
    		break;
    	}
    	complete = i / percent;
    	if(complete != prev_complete){
    		xil_printf("%d%%\r",complete);
    		prev_complete = complete;
    	}
    }
    //-------------------------------------------------------------------------
    if(!error){
    	xil_printf("Filling memory with pattern2...\n\r");
    	percent = test_size / 100;
    	prev_complete = 1;
    	for(i = 0; i < test_size; i++){
    		ddr_mem[i] = pattern2;
    		complete = i / percent;
    		if(complete != prev_complete){
    			xil_printf("%d%%\r",complete);
    			prev_complete = complete;
    		}
    	}

    	xil_printf("Checking...\n\r");
    	prev_complete = 1;
    	for(i = 0; i < test_size; i++){
    		if(ddr_mem[i] != pattern2){
    			xil_printf("Test Failed @%04X %04X != %04X",i,ddr_mem[i],pattern2);
    			error = 1;
    			break;
    		}
    		complete = i / percent;
    		if(complete != prev_complete){
    			xil_printf("%d%%\r",complete);
    			prev_complete = complete;
    		}
    	}
    }
    //-------------------------------------------------------------------------
    if(!error){
    	xil_printf("Filling memory with counter...\n\r");
    	percent = test_size / 100;
    	prev_complete = 1;
    	for(i = 0; i < test_size; i++){
    		ddr_mem[i] = i;
    		complete = i / percent;
    		if(complete != prev_complete){
    			xil_printf("%d%%\r",complete);
    			prev_complete = complete;
    		}
    	}

    	xil_printf("Checking...\n\r");
    	prev_complete = 1;
    	for(i = 0; i < test_size; i++){
    		if(ddr_mem[i] != i){
    			xil_printf("Test Failed @%04X %04X != %04X",i,ddr_mem[i],i);
    			error = 1;
    			break;
    		}
    		complete = i / percent;
    		if(complete != prev_complete){
    			xil_printf("%d%%\r",complete);
    			prev_complete = complete;
    		}
    	}
    }
    //-------------------------------------------------------------------------

    if(!error)
    	xil_printf("Passed\n\r");

    return 0;
}
