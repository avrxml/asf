/*
*********************************************************************************************************
*                                               uC/CPU
*                                    CPU CONFIGURATION & PORT LAYER
*
*                          (c) Copyright 2004-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/CPU is provided in source form for FREE evaluation, for educational
*               use or peaceful research.  If you plan on using uC/CPU in a commercial
*               product you need to contact Micrium to properly license its use in your
*               product.  We provide ALL the source code for your convenience and to
*               help you experience uC/CPU.  The fact that the source code is provided
*               does NOT mean that you can use it without paying a licensing fee.
*
*               Knowledge of the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            CPU PORT FILE
*
*                                           Atmel AVR32 UC3
*                                            IAR C Compiler
*
* Filename      : cpu_a.asm
* Version       : V1.18
* Programmer(s) : FGK
*                 ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             ASM HEADER
*********************************************************************************************************
*/

        MODULE  CPU_A

        RSEG    CODE32:CODE

        EXTERN  __program_start


;$PAGE
/*
*********************************************************************************************************
*                                              DEFINES
*********************************************************************************************************
*/

CPU_SR_OFFSET              EQU                             0    /* Status  Register offset in System Register           */
CPU_SR_GM_OFFSET           EQU                            16    /* Status  Register, Global Interrupt Mask Offset       */
CPU_SR_GM_MASK             EQU                    0x00010000    /* Status  Register, Global Interrupt Mask              */
CPU_SR_EM_OFFSET           EQU                            21    /* Status  Register, Exception Mask Offset              */
CPU_SR_M0_MASK             EQU                    0x00400000    /* Status  Register, Supervisor Execution Mode Mask     */
CPU_SR_MX_OFFSET           EQU                            22    /* Status  Register, Execution Mode Mask offset         */
CPU_SR_MX_SUPERVISOR_MODE  EQU                    0x00000001    /* Status  Register, Execution Mode Supervisor          */
CPU_COUNT_OFFSET           EQU                    0x00000108    /* Count   Register offset in System Register           */
CPU_CONFIG0_OFFSET         EQU                    0x00000100    /* Config0 Register offset in System Register           */
CPU_COMPARE_OFFSET         EQU                    0x0000010C    /* Compare Register offset in System Register           */


/*
*********************************************************************************************************
*                                          PUBLIC DECLARATIONS
*********************************************************************************************************
*/

        PUBLIC  CPU_SR_Save
        PUBLIC  CPU_SR_Restore
        PUBLIC  CPU_IntDis
        PUBLIC  CPU_IntEn
        PUBLIC  CPU_ExceptDis
        PUBLIC  CPU_ExceptEn
        PUBLIC  CPU_Reset
        PUBLIC  CPU_SysReg_Get_Count
        PUBLIC  CPU_SysReg_Get_Config0
        PUBLIC  CPU_SysReg_Set_Compare
        PUBLIC  CPU_SysReg_Get_Compare
        PUBLIC  CPU_CntLeadZeros


;$PAGE
/*
*********************************************************************************************************
*                                      CRITICAL SECTION FUNCTIONS
*
* Description : These functions are used to enter and exit critical sections using Critical Method #3.
*
*                   CPU_SR  CPU_SR_Save (void)
*                          Get current global interrupt mask bit value from SR
*                          Disable interrupts
*                          Return global interrupt mask bit
*
*                   void  CPU_SR_Restore (CPU_SR  cpu_sr)
*                          Set global interrupt mask bit on SR according to parameter sr
*                          Return
*
* Argument(s) : cpu_sr      global interrupt mask status.
*
* Note(s)     : (1) Besides global interrupt mask bit, all other status register bits are kept unchanged.
*
*               (2) Two NOP are required for properly disable interrupts.
*********************************************************************************************************
*/

CPU_SR_Save:
        CSRFCZ  CPU_SR_GM_OFFSET                                /* Retrieve GM bit from SR                              */
        SRCS    R12                                             /* if (GM == 1) set R12                                 */
        SSRF    CPU_SR_GM_OFFSET                                /* set global interrupt mask (disable interrupts)       */
        NOP
        NOP
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */


CPU_SR_Restore:
        PUSHM   R11                                             /* Save R11 into stack                                  */

        MFSR    R11, CPU_SR_OFFSET                              /* Retrieve current Status Register                     */
        LSR     R12, 1                                          /* Copy interrupt status to Carry                       */
        BST     R11, CPU_SR_GM_OFFSET                           /* Overwrite GM bit based on Carry                      */

        MTSR    CPU_SR_OFFSET, R11                              /* Restore Status Register GM with previous interrupt   */
                                                                /* ... status value                                     */

        POPM    R11                                             /* Restore R11 from stack                               */
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */


;$PAGE
/*
*********************************************************************************************************
*                                      DISABLE/ENABLE INTERRUPTS
*
* Description : Disable/Enable interrupts by setting or clearing the global interrupt mask in the cpu
*               status register.
*
*                    void  CPU_IntDis (void)
*                           Set global interrupt mask bit on SR
*                           Return
*
*                    void  CPU_IntEn (void)
*                           Clear global interrupt mask bit on SR
*                           Return
*********************************************************************************************************
*/

CPU_IntDis:
        SSRF    CPU_SR_GM_OFFSET                                /* set global interrupt mask (disable interrupts)       */
        NOP
        NOP
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */


CPU_IntEn:
        CSRF    CPU_SR_GM_OFFSET                                /* clear global interrupt mask (enable interrupts)      */
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */


;$PAGE
/*
*********************************************************************************************************
*                                      DISABLE/ENABLE EXCEPTIONS
*
* Description : These functions are used to disable and enable exceptions by setting or clearing the
*               exception mask in the cpu status register.
*
*                   void  CPU_ExceptDis (void)
*                          Set exception mask bit on SR
*                          Return
*
*                   void  CPU_ExcepttEn (void)
*                          Clear exception mask bit on SR
*                          Return
*********************************************************************************************************
*/

CPU_ExceptDis:
        SSRF    CPU_SR_EM_OFFSET                                /* set exception mask (disable exceptions)              */
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */


CPU_ExceptEn:
        CSRF    CPU_SR_EM_OFFSET                                /* clear exceptions mask (enable exceptions)            */
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */


;$PAGE
/*
*********************************************************************************************************
*                                              RESET CPU
*
* Description : This function is used to reset the CPU by returning to the reset vector.
*
*               void  CPU_Reset (void)
*                      if (current SR == 001b) {
*                          Push       PC  (START)
*                          Push clean SR  (GM | M0)
*                          RETS
*                      } else {
*                          Push clean R8  (0x08080808)
*                          Push clean R9  (0x09090909)
*                          Push clean R10 (0x10101010)
*                          Push clean R11 (0x11111111)
*                          Push clean R12 (0x00000000)
*                          Push clean LR  (0x14141414)
*                          Push       PC  (START)
*                          Push clean SR  (GM | M0)
*                          RETE
*                      }
*********************************************************************************************************
*/

CPU_Reset:
        MFSR    R8, CPU_SR_OFFSET
        BFEXTU  R8, R8, CPU_SR_MX_OFFSET, 3
        CP.W    R8, CPU_SR_MX_SUPERVISOR_MODE
        BRNE    CPU_Reset_RETE

        MOV     R8, LWRD(__program_start)
        ORH     R8, HWRD(__program_start)
        MOV     R9, LWRD(CPU_SR_GM_MASK | CPU_SR_M0_MASK)
        ORH     R9, HWRD(CPU_SR_GM_MASK | CPU_SR_M0_MASK)
        STM     --SP, R8-R9                                     /* Push PC and SR                                       */
        RETS


CPU_Reset_RETE:
        MOV     R8,  0x0808
        ORH     R8,  0x0808
        MOV     R9,  0x0909
        ORH     R9,  0x0909
        MOV     R10, 0x1010
        ORH     R10, 0x1010
        MOV     R11, 0x1111
        ORH     R11, 0x1111
        MOV     R12, 0x0000
        ORH     R12, 0x0000
        MOV     LR,  0x1414
        ORH     LR,  0x1414
        STM     --SP, R8-R12, LR                                /* Push R8-R12, LR                                      */
        MOV     R8, LWRD(__program_start)
        ORH     R8, HWRD(__program_start)
        MOV     R9, LWRD(CPU_SR_GM_MASK | CPU_SR_M0_MASK)
        ORH     R9, HWRD(CPU_SR_GM_MASK | CPU_SR_M0_MASK)
        STM     --SP, R8-R9                                     /* Push PC and SR                                       */
        RETE


;$PAGE
/*
*********************************************************************************************************
*                                      GET/SET SYSTEM REGISTERS
*
* Description : These functions are used to get/set specific system registers.
*
*                   CPU_INT32U   CPU_SysReg_Get_Count (void)
*                   CPU_INT32U   CPU_SysReg_Get_Config0 (void)
*                   CPU_INT32U   CPU_SysReg_Get_Compare (void)
*                   void         CPU_SysReg_Set_Compare (CPU_INT32U value)
*********************************************************************************************************
*/

CPU_SysReg_Get_Count:
        MFSR    R12, CPU_COUNT_OFFSET                           /* Retrieve COUNT system register                       */
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */


CPU_SysReg_Get_Config0:
        MFSR    R12, CPU_CONFIG0_OFFSET                         /* Retrieve CONFIG0 system register                     */
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */


CPU_SysReg_Get_Compare:
        MFSR    R12, CPU_COMPARE_OFFSET                         /* Retrieve COMPARE system register                     */
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */


CPU_SysReg_Set_Compare:
        MTSR    CPU_COMPARE_OFFSET, R12                         /* Save COMPARE system register                         */
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */


;$PAGE
/*
*********************************************************************************************************
*                                         COUNT LEADING ZEROS
*
* Description : Counts the number of binary zero bits before the first binary one bit in the argument.
*               If the argument is zero, the value 32 is returned.
*
*               CPU_INT32U  CPU_CntLeadZeros (CPU_INT32U  val)
*
* Argument(s) : val     variable to count leading zeros
*********************************************************************************************************
*/

CPU_CntLeadZeros:
        CLZ     R12, R12                                        /* Count leading zeros                                  */
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */


;$PAGE
/*
*********************************************************************************************************
*                                     CPU ASSEMBLY PORT FILE END
*********************************************************************************************************
*/

                END
