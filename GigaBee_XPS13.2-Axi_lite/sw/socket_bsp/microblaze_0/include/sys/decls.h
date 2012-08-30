////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004 Xilinx, Inc.  All rights reserved.
//
// Xilinx, Inc.
// XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
// COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
// ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
// STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
// IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
// FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
// XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
// THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
// ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
// FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Id: decls.h,v 1.1.2.1 2010/01/04 19:30:42 vasanth Exp $
//
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------//
//! @file decls.h
//! Gathers declarations for kernel data structures in one place
//----------------------------------------------------------------------------------------------------//
#ifndef _SYS_DECLS_H
#define _SYS_DECLS_H

#ifdef __cplusplus
extern "C" {
#endif

extern char kernel_irq_stack[];
extern char process_startup_stack[];

#include <os_config.h>
#include <sys/arch.h>
#include <sys/types.h>
#include <sys/ktypes.h>

extern pid_t            current_pid, prev_pid, idle_task_pid;
extern process_struct   *current_process;
extern process_struct   ptable[];
extern reent_t          reent;
extern void             *_stack, __stack;
extern struct _queue    ready_q[];       
extern void             *kernel_sp;  
extern pid_t            idle_task_pid;

extern void             idle_task (void);

#define kerrno          (current_process->reent._errno)

#ifdef VERBOSE
#define DBG_PRINT(string) print(string)
#define DPUTNUM(num)      putnum(num)
#define DPRINTF           xil_printf
#else
#define DBG_PRINT(string) // nothing
#define DPUTNUM(num)      // nothing
#define DPRINTF(...)      // nothing
#endif

#ifdef __cplusplus
}       
#endif 

#endif /* _SYS_DECLS_H */
