#include <stdio.h>
#include "xparameters.h"
#include "xbasic_types.h"
#include "xil_cache.h"

//#include "xuartns550.h"

//XUartNs550 UARTInst;

u32 pattern1 = 0xA5A5A5A5;
u32 pattern2 = 0x5A5A5A5A;
volatile u32 *mcb3_ddr3_mem = (u32*)XPAR_MCB_DDR3_S0_AXI_BASEADDR;
volatile u32 *mcb1_ddr3_mem = (u32*)XPAR_MCB1_DDR3_S0_AXI_BASEADDR;


int main()
{
	u32 i, percent, complete, prev_complete;
	// Both memory blocks have equal size so we can use same value
	u32 test_size = (XPAR_MCB_DDR3_S0_AXI_HIGHADDR - XPAR_MCB_DDR3_S0_AXI_BASEADDR)/sizeof(u32);
//	u32 test_size = 10000;
	u32 error = 0;

#if XPAR_MICROBLAZE_USE_ICACHE
	Xil_ICacheInvalidate();
	Xil_ICacheEnable();
#endif
#if XPAR_MICROBLAZE_USE_DCACHE
	Xil_DCacheInvalidate();
	Xil_DCacheEnable();
#endif

    //XUartNs550_Initialize(&UARTInst, XPAR_RS232_UART_1_DEVICE_ID);
    //XUartNs550_SetBaud(XPAR_RS232_UART_1_BASEADDR, XPAR_RS232_UART_1_CLOCK_FREQ_HZ, 115200);
    xil_printf("\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r");
    xil_printf("==========================================================\n\r");
    xil_printf("                      Memory test\n\r");
    xil_printf("==========================================================\n\r");
    xil_printf("Base Address : 0x%04X\r\n",XPAR_MCB_DDR3_S0_AXI_BASEADDR);
    xil_printf("High Address : 0x%04X\r\n\r\n",XPAR_MCB_DDR3_S0_AXI_HIGHADDR);
    //-------------------------------------------------------------------------
    xil_printf("Filling memory with pattern1...\n\r");
    percent = test_size / 100;
    prev_complete = 1;
    for(i = 0; i < test_size; i++){
    	mcb3_ddr3_mem[i] = pattern1;
    	complete = i / percent;
    	if(complete != prev_complete){
    		xil_printf("%d%%\r",complete);
    		prev_complete = complete;
    	}
    }

    xil_printf("Checking...\n\r");
    prev_complete = 1;
    for(i = 0; i < test_size; i++){
    	if(mcb3_ddr3_mem[i] != pattern1){
    		xil_printf("Test Failed @%04X 0x%08X != %08X",i,mcb3_ddr3_mem[i],pattern1);
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
    		mcb3_ddr3_mem[i] = pattern2;
    		complete = i / percent;
    		if(complete != prev_complete){
    			xil_printf("%d%%\r",complete);
    			prev_complete = complete;
    		}
    	}

    	xil_printf("Checking...\n\r");
    	prev_complete = 1;
    	for(i = 0; i < test_size; i++){
    		if(mcb3_ddr3_mem[i] != pattern2){
    			xil_printf("Test Failed @%04X 0x%08X != %08X",i,mcb3_ddr3_mem[i],pattern2);
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
    		mcb3_ddr3_mem[i] = i;
    		complete = i / percent;
    		if(complete != prev_complete){
    			xil_printf("%d%%\r",complete);
    			prev_complete = complete;
    		}
    	}

    	xil_printf("Checking...\n\r");
    	prev_complete = 1;
    	for(i = 0; i < test_size; i++){
    		if(mcb3_ddr3_mem[i] != i){
    			xil_printf("Test Failed @%04X 0x%08X != %08X",i,mcb3_ddr3_mem[i],i);
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
    // Second Block
    xil_printf("----------------------------------------------------------\n\r");
    xil_printf("Base Address : 0x%04X\r\n",XPAR_MCB1_DDR3_S0_AXI_BASEADDR);
    xil_printf("High Address : 0x%04X\r\n\r\n",XPAR_MCB1_DDR3_S0_AXI_HIGHADDR);
    //-------------------------------------------------------------------------
    xil_printf("Filling memory with pattern1...\n\r");
    percent = test_size / 100;
    prev_complete = 1;
    for(i = 0; i < test_size; i++){
    	mcb1_ddr3_mem[i] = pattern1;
    	complete = i / percent;
    	if(complete != prev_complete){
    		xil_printf("%d%%\r",complete);
    		prev_complete = complete;
    	}
    }

    xil_printf("Checking...\n\r");
    prev_complete = 1;
    for(i = 0; i < test_size; i++){
    	if(mcb1_ddr3_mem[i] != pattern1){
    		xil_printf("Test Failed @%04X 0x%08X != %08X",i,mcb1_ddr3_mem[i],pattern1);
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
    		mcb1_ddr3_mem[i] = pattern2;
    		complete = i / percent;
    		if(complete != prev_complete){
    			xil_printf("%d%%\r",complete);
    			prev_complete = complete;
    		}
    	}

    	xil_printf("Checking...\n\r");
    	prev_complete = 1;
    	for(i = 0; i < test_size; i++){
    		if(mcb1_ddr3_mem[i] != pattern2){
    			xil_printf("Test Failed @%04X 0x%08X != %08X",i,mcb1_ddr3_mem[i],pattern2);
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
    		mcb1_ddr3_mem[i] = i;
    		complete = i / percent;
    		if(complete != prev_complete){
    			xil_printf("%d%%\r",complete);
    			prev_complete = complete;
    		}
    	}

    	xil_printf("Checking...\n\r");
    	prev_complete = 1;
    	for(i = 0; i < test_size; i++){
    		if(mcb1_ddr3_mem[i] != i){
    			xil_printf("Test Failed @%04X 0x%08X != %08X",i,mcb1_ddr3_mem[i],i);
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
