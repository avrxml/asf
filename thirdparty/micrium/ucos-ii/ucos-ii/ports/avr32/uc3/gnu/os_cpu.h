/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                     ATMEL  AVR32 UC3 Specific code
*
*                                 (c) Copyright 2007; Micrium; Weston, FL
*                                           All Rights Reserved
*
* File    : OS_CPU.H
* By      : Fabiano Kovalski
*
* LICENSING TERMS:
* ---------------
*   uC/OS-II is provided in source form for FREE evaluation, for educational use or for peaceful research.
* If you plan on using  uC/OS-II  in a commercial product you need to contact Micrium to properly  license
* its use in your product.  We provide ALL the source code for your convenience and to help you experience
* uC/OS-II.   The fact that the  source  is provided does  NOT  mean that you can use it without  paying a
* licensing fee.
*********************************************************************************************************
*/

#include  <cpu.h>

#ifdef    OS_CPU_GLOBALS
#define   OS_CPU_EXT
#else
#define   OS_CPU_EXT  extern
#endif

/*
**********************************************************************************************************
*                                           DATA TYPES
**********************************************************************************************************
*/

typedef  CPU_BOOLEAN    BOOLEAN;
typedef  CPU_INT08U     INT8U;                                          /* Unsigned  8 bit quantity                                 */
typedef  CPU_INT08S     INT8S;                                          /* Signed    8 bit quantity                                 */
typedef  CPU_INT16U     INT16U;                                         /* Unsigned 16 bit quantity                                 */
typedef  CPU_INT16S     INT16S;                                         /* Signed   16 bit quantity                                 */
typedef  CPU_INT32U     INT32U;                                         /* Unsigned 32 bit quantity                                 */
typedef  CPU_INT32S     INT32S;                                         /* Signed   32 bit quantity                                 */
typedef  CPU_FP32       FP32;                                           /* Single precision floating point                          */
typedef  CPU_FP64       FP64;                                           /* Double precision floating point                          */

typedef  CPU_STK        OS_STK;                                         /* Define size of CPU stack entry                           */
typedef  CPU_SR         OS_CPU_SR;                                      /* Define size of CPU status register                       */

/*
*********************************************************************************************************
*                                           Critical Method MACROS
*********************************************************************************************************
*/

#define  OS_CRITICAL_METHOD     CPU_CFG_CRITICAL_METHOD

#define  OS_ENTER_CRITICAL()   {CPU_CRITICAL_ENTER();}
#define  OS_EXIT_CRITICAL()    {CPU_CRITICAL_EXIT();}

/*
**********************************************************************************************************
*                                          Miscellaneous
**********************************************************************************************************
*/

#define  OS_STK_GROWTH          1                                       /* Stack grows from HIGH to LOW memory on AVR               */

#define  OS_TASK_SW()          __asm__ __volatile__ ("scall")           /* Perform task switch by a supervisor call (exception)     */

/*
**********************************************************************************************************
*                                         Function Prototypes
**********************************************************************************************************
*/

void  OSIntCtxSw(void);
void  OSStartHighRdy(void);

void  OSCtxRestore(INT32U *sp);
void  OSIntCtxRestore(INT32U *sp);
