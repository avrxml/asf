/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                    ATMEL AVR32 UC3 Exception Vectors
*
*                                 (c) Copyright 2007; Micrium; Weston, FL
*                                           All Rights Reserved
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        EXCEPTION VECTOR FILE
*
*                                           Atmel AVR32 UC3
*                                            IAR C Compiler
*
* Filename      : vectors.asm
* Programmer(s) : FGK
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             ASM HEADER
*********************************************************************************************************
*/
        MODULE  EXCEPTION_VECTORS
        COMMON  EVTAB:CODE:NOROOT

        EXTERN  ??init_EVBA
        REQUIRE ??init_EVBA                                             /* Forces EVBA initialization                               */

        PUBLIC  ??EVBA
        PUBLIC  _evba


;$PAGE
/*
*********************************************************************************************************
*                                          DEFINES
*********************************************************************************************************
*/

VECTORS_SR_OFFSET               EQU              0                      /* Status  Register offset in System Register               */
VECTORS_SR_GM_OFFSET            EQU             16                      /* Status  Register, Global Interrupt Mask Offset           */
VECTORS_SR_MX_OFFSET            EQU             22                      /* Status  Register, Execution Mode Mask offset             */
VECTORS_SR_MX_EXCEPTION_MODE    EQU     0x00000006                      /* Status  Register, Execution Mode Exception               */

VECTORS_INT0                    EQU              0                      /* Lowest interrupt priority level                          */
VECTORS_INT1                    EQU              1
VECTORS_INT2                    EQU              2
VECTORS_INT3                    EQU              3                      /* Highest interrupt priority level                         */


;$PAGE
/*
*********************************************************************************************************
*                            EXCEPTION VECTOR BASE
*
* Description: This section is used to build the exception vector base. Each different exception points
*              to a specific handler.
*
* Note(s)    : Generally, this section does not need to be modified. If an exception needs to be handled
*              differently, only the handler needs to be modified accordingly.
*********************************************************************************************************
*/
??EVBA:
_evba:
        ORG     0x000                                                   /* Unrecoverable Exception                                  */
        BRAL    _handle_Unrecoverable_Exception

        ORG     0x004                                                   /* TLB Multiple Hit: UNUSED IN AVR32A                       */
        BRAL    _handle_TLB_Multiple_Hit

        ORG     0x008                                                   /* Bus Error Data Fetch                                     */
        BRAL    _handle_Bus_Error_Data_Fetch

        ORG     0x00C                                                   /* Bus Error Instruction Fetch                              */
        BRAL    _handle_Bus_Error_Instruction_Fetch

        ORG     0x010                                                   /* NMI                                                      */
        BRAL    _handle_NMI

        ORG     0x014                                                   /* Instruction Address                                      */
        BRAL    _handle_Instruction_Address

        ORG     0x018                                                   /* ITLB Protection                                          */
        BRAL    _handle_ITLB_Protection

        ORG     0x01C                                                   /* Breakpoint                                               */
        BRAL    _handle_Breakpoint

        ORG     0x020                                                   /* Illegal Opcode                                           */
        BRAL    _handle_Illegal_Opcode

        ORG     0x024                                                   /* Unimplemented Instruction                                */
        BRAL    _handle_Unimplemented_Instruction

        ORG     0x028                                                   /* Privilege Violation                                      */
        BRAL    _handle_Privilege_Violation

        ORG     0x02C                                                   /* Floating-Point: UNUSED IN AVR32A                         */
        BRAL    _handle_Floating_Point

        ORG     0x030                                                   /* Coprocessor Absent: UNUSED IN AVR32A                     */
        BRAL    _handle_Coprocessor_Absent

        ORG     0x034                                                   /* Data Address (Read)                                      */
        BRAL    _handle_Data_Address_Read

        ORG     0x038                                                   /* Data Address (Write)                                     */
        BRAL    _handle_Data_Address_Write

        ORG     0x03C                                                   /* DTLB Protection (Read)                                   */
        BRAL    _handle_DTLB_Protection_Read

        ORG     0x040                                                   /* DTLB Protection (Write)                                  */
        BRAL    _handle_DTLB_Protection_Write

        ORG     0x044                                                   /* DTLB Modified: UNUSED IN AVR32A                          */
        BRAL    _handle_DTLB_Modified

        ORG     0x050                                                   /* ITLB Miss: UNUSED IN AVR32A                              */
        BRAL    _handle_ITLB_Miss

        ORG     0x060                                                   /* DTLB Miss (Read): UNUSED IN AVR32A                       */
        BRAL    _handle_DTLB_Miss_Read

        ORG     0x070                                                   /* DTLB Miss (Write): UNUSED IN AVR32A                      */
        BRAL    _handle_DTLB_Miss_Write

        ORG     0x100                                                   /* Supervisor Call                                          */
        BRAL    _handle_Supervisor_Call


;$PAGE
/*
*********************************************************************************************************
*                            EXCEPTION HANDLERS
*
* Description: This section is used to define the exception handlers. Each exception has its own handler.
*
* Note(s)    : Unhandled exceptions can be caught by __unhandled_exception debug function.
*              The function is prototyped as:
*                       void __unhandled_exception(void* offending_PC, unsigned int vector_number);
*                       vector_number       is passed thru R11
*                       offending_PC        is retrieved from the stack
*
*              Exceptions can also force a cpu reset by executing the function CPU_Reset. In order to add this
*              functionality, first EXTERN CPU_Reset and then replace the handler code with a branch to CPU_Reset.
*
*              Any subroutine call inside a handler alters the Link Register, be sure to restore its old value
*              once the subroutine returns.
*********************************************************************************************************
*/
        RSEG    EVSEG:CODE

        ALIGN   4

        EXTERN  __unhandled_exception
        EXTERN  OSCtxSw

_handle_Unrecoverable_Exception:
        PUSHM   R11
        MOV     R11, 0x000                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_TLB_Multiple_Hit:
        PUSHM   R11
        MOV     R11, 0x004                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_Bus_Error_Data_Fetch:
        PUSHM   R11
        MOV     R11, 0x008                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_Bus_Error_Instruction_Fetch:
        PUSHM   R11
        MOV     R11, 0x00C                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_NMI:
        PUSHM   R11
        MOV     R11, 0x010                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_Instruction_Address:
        PUSHM   R11
        MOV     R11, 0x014                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_ITLB_Protection:
        PUSHM   R11
        MOV     R11, 0x018                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_Breakpoint:
        PUSHM   R11
        MOV     R11, 0x01C                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_Illegal_Opcode:
        PUSHM   R11
        MOV     R11, 0x020                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_Unimplemented_Instruction:
        PUSHM   R11
        MOV     R11, 0x024                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_Privilege_Violation:
        PUSHM   R11
        MOV     R11, 0x028                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_Floating_Point:
        PUSHM   R11
        MOV     R11, 0x02C                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_Coprocessor_Absent:
        PUSHM   R11
        MOV     R11, 0x030                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_Data_Address_Read:
        PUSHM   R11
        MOV     R11, 0x034                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_Data_Address_Write:
        PUSHM   R11
        MOV     R11, 0x038                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_DTLB_Protection_Read:
        PUSHM   R11
        MOV     R11, 0x03C                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_DTLB_Protection_Write:
        PUSHM   R11
        MOV     R11, 0x040                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_DTLB_Modified:
        PUSHM   R11
        MOV     R11, 0x044                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_ITLB_Miss:
        PUSHM   R11
        MOV     R11, 0x050                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_DTLB_Miss_Read:
        PUSHM   R11
        MOV     R11, 0x060                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_DTLB_Miss_Write:
        PUSHM   R11
        MOV     R11, 0x070                                              /* vector_number for __unhandled_exception                  */
        BRAL    __unhandled_exception

_handle_Supervisor_Call:
        LDDPC   PC, __OSCtxSw


        ALIGN   4
__OSCtxSw:                                                              /* 32-bit jump reference for OSCtxSw to be used with LDDPC  */
        DC32    OSCtxSw


;$PAGE
/*
*********************************************************************************************************
*                                          INTERRUPT SUPPORT
*
* Description: This variable externs interrupt auto-vector offsets to IntX.
*********************************************************************************************************
*/
        PUBLIC  OSIntPrioReg

        ALIGN   4

OSIntPrioReg:
        DC32    (Int0  - _evba)
        DC32    (Int1  - _evba)
        DC32    (Int2  - _evba)
        DC32    (Int3  - _evba)
        DC32    (IntF0 - _evba)
        DC32    (IntF1 - _evba)
        DC32    (IntF2 - _evba)
        DC32    (IntF3 - _evba)


;$PAGE
/*
*********************************************************************************************************
*                                          INTERRUPT SUPPORT
*
* Description: These functions are the 4 different interrupt handlers available. One for each priority level.
*
* Notes      : 1) The address of Int0 is loaded into all INTC IPR by the BSP_INTC_Init().
*
*              2) BSP_INTC_IntReg() updates INTC IPR with the proper interrupt priority handler address given
*                 the priority level chosen.
*
*              3) The stack frame of the interrupted task looks as follows:
*
*                                        SP --> SR           (Low Memory)
*                                               PC
*                                               LR
*                                               R12
*                                               R11
*                                               R10
*                                               R9
*                                               R8   ----\   (High memory)
*
*              3) If an Exception or an NMI execution mode are detected, the interrupt handler must restore
*                 the stack to an exception stack and return to exception handler.
*
*              4) The stack frame adjusted to an exception stack looks as follows:
*
*                                               SR           (Low Memory)
*                                               PC
*                                               LR
*                                               R12
*                                               R11
*                                               R10
*                                        SP --> SR
*                                               PC   ----\   (High memory)
*
*              5) IntX() MUST:
*                   a) Check execution mode,
*                   b) If execution mode is Exception or NMI then change interrupt stack to exception stack and
*                      return to the exception handler,
*                   c) Call BSP_INTC_IntGetHandler(int_level) returning an user-defined ISR function to be called,
*                   d) If an user-defined function is returned then jump to OSIntISRHandler,
*                   e) return from event handler.
*********************************************************************************************************
*/
        RSEG    EVSEG:CODE

        PUBLIC  Int0
        PUBLIC  Int1
        PUBLIC  Int2
        PUBLIC  Int3
        PUBLIC  IntF0
        PUBLIC  IntF1
        PUBLIC  IntF2
        PUBLIC  IntF3

        EXTERN  OSIntISRHandler
        EXTERN  OSFastIntISRHandler
        EXTERN  BSP_INTC_IntGetHandler

        ALIGN   4

Int0:
        SSRF    VECTORS_SR_GM_OFFSET                                    /* Set global interrupt mask (disable interrupts)           */
        NOP
        NOP

        MFSR    R12, VECTORS_SR_OFFSET
        BFEXTU  R12, R12, VECTORS_SR_MX_OFFSET, 3
        CP.W    R12, VECTORS_SR_MX_EXCEPTION_MODE                       /* check execution mode                                     */
        BRHS    __exception_stack                                       /* if (R12 == {Exception, NMI}) then change interrupt stack */
                                                                        /* ... to exception stack                                   */

        MOV     R12, VECTORS_INT0                                       /* Pass int_level parameter to BSP_INTC_IntGetHandler       */
        RCALL   BSP_INTC_IntGetHandler
        CP.W    R12, 0                                                  /* Test returned interrupt handler pointer                  */
        BRNE    OSIntISRHandler                                         /* if (R12 != 0) then, jump to the OSIntISRHandler          */
        RETE                                                            /* if (R12 == 0) it was a spurious interrupt then, return   */
                                                                        /* ... from event handler                                   */

Int1:
        SSRF    VECTORS_SR_GM_OFFSET                                    /* Set global interrupt mask (disable interrupts)           */
        NOP
        NOP

        MFSR    R12, VECTORS_SR_OFFSET
        BFEXTU  R12, R12, VECTORS_SR_MX_OFFSET, 3
        CP.W    R12, VECTORS_SR_MX_EXCEPTION_MODE                       /* check execution mode                                     */
        BRHS    __exception_stack                                       /* if (R12 == {Exception, NMI}) then change interrupt stack */
                                                                        /* ... to exception stack                                   */

        MOV     R12, VECTORS_INT1                                       /* Pass int_level parameter to BSP_INTC_IntGetHandler       */
        RCALL   BSP_INTC_IntGetHandler
        CP.W    R12, 0                                                  /* Test returned interrupt handler pointer                  */
        BRNE    OSIntISRHandler                                         /* if (R12 != 0) then, jump to the OSIntISRHandler          */
        RETE                                                            /* if (R12 == 0) it was a spurious interrupt then, return   */
                                                                        /* ... from event handler                                   */

Int2:
        SSRF    VECTORS_SR_GM_OFFSET                                    /* Set global interrupt mask (disable interrupts)           */
        NOP
        NOP

        MFSR    R12, VECTORS_SR_OFFSET
        BFEXTU  R12, R12, VECTORS_SR_MX_OFFSET, 3
        CP.W    R12, VECTORS_SR_MX_EXCEPTION_MODE                       /* check execution mode                                     */
        BRHS    __exception_stack                                       /* if (R12 == {Exception, NMI}) then change interrupt stack */
                                                                        /* ... to exception stack                                   */

        MOV     R12, VECTORS_INT2                                       /* Pass int_level parameter to BSP_INTC_IntGetHandler       */
        RCALL   BSP_INTC_IntGetHandler
        CP.W    R12, 0                                                  /* Test returned interrupt handler pointer                  */
        BRNE    OSIntISRHandler                                         /* if (R12 != 0) then, jump to the OSIntISRHandler          */
        RETE                                                            /* if (R12 == 0) it was a spurious interrupt then, return   */
                                                                        /* ... from event handler                                   */

Int3:
        SSRF    VECTORS_SR_GM_OFFSET                                    /* Set global interrupt mask (disable interrupts)           */
        NOP
        NOP

        MFSR    R12, VECTORS_SR_OFFSET
        BFEXTU  R12, R12, VECTORS_SR_MX_OFFSET, 3
        CP.W    R12, VECTORS_SR_MX_EXCEPTION_MODE                       /* check execution mode                                     */
        BRHS    __exception_stack                                       /* if (R12 == {Exception, NMI}) then change interrupt stack */
                                                                        /* ... to exception stack                                   */

        MOV     R12, VECTORS_INT3                                       /* Pass int_level parameter to BSP_INTC_IntGetHandler       */
        RCALL   BSP_INTC_IntGetHandler
        CP.W    R12, 0                                                  /* Test returned interrupt handler pointer                  */
        BRNE    OSIntISRHandler                                         /* if (R12 != 0) then, jump to the OSIntISRHandler          */
        RETE                                                            /* if (R12 == 0) it was a spurious interrupt then, return   */
                                                                        /* ... from event handler                                   */

IntF0:
        SSRF    VECTORS_SR_GM_OFFSET                                    /* Set global interrupt mask (disable interrupts)           */
        NOP
        NOP

        MOV     R12, VECTORS_INT0                                       /* Pass int_level parameter to BSP_INTC_IntGetHandler       */
        RCALL   BSP_INTC_IntGetHandler
        CP.W    R12, 0                                                  /* Test returned interrupt handler pointer                  */
        BRNE    OSFastIntISRHandler                                     /* if (R12 != 0) then, jump to the OSFastIntISRHandler      */
        RETE                                                            /* if (R12 == 0) it was a spurious interrupt then, return   */
                                                                        /* ... from event handler                                   */

IntF1:
        SSRF    VECTORS_SR_GM_OFFSET                                    /* Set global interrupt mask (disable interrupts)           */
        NOP
        NOP

        MOV     R12, VECTORS_INT1                                       /* Pass int_level parameter to BSP_INTC_IntGetHandler       */
        RCALL   BSP_INTC_IntGetHandler
        CP.W    R12, 0                                                  /* Test returned interrupt handler pointer                  */
        BRNE    OSFastIntISRHandler                                     /* if (R12 != 0) then, jump to the OSFastIntISRHandler      */
        RETE                                                            /* if (R12 == 0) it was a spurious interrupt then, return   */
                                                                        /* ... from event handler                                   */

IntF2:
        SSRF    VECTORS_SR_GM_OFFSET                                    /* Set global interrupt mask (disable interrupts)           */
        NOP
        NOP

        MOV     R12, VECTORS_INT2                                       /* Pass int_level parameter to BSP_INTC_IntGetHandler       */
        RCALL   BSP_INTC_IntGetHandler
        CP.W    R12, 0                                                  /* Test returned interrupt handler pointer                  */
        BRNE    OSFastIntISRHandler                                     /* if (R12 != 0) then, jump to the OSFastIntISRHandler      */
        RETE                                                            /* if (R12 == 0) it was a spurious interrupt then, return   */
                                                                        /* ... from event handler                                   */

IntF3:
        SSRF    VECTORS_SR_GM_OFFSET                                    /* Set global interrupt mask (disable interrupts)           */
        NOP
        NOP

        MOV     R12, VECTORS_INT3                                       /* Pass int_level parameter to BSP_INTC_IntGetHandler       */
        RCALL   BSP_INTC_IntGetHandler
        CP.W    R12, 0                                                  /* Test returned interrupt handler pointer                  */
        BRNE    OSFastIntISRHandler                                     /* if (R12 != 0) then, jump to the OSFastIntISRHandler      */
        RETE                                                            /* if (R12 == 0) it was a spurious interrupt then, return   */
                                                                        /* ... from event handler                                   */

__exception_stack:
        LDDSP   R12, SP[0 * 4]                                          /* Retrieve SR from stack                                   */
        STDSP   SP[6 * 4], R12                                          /* Store SR at bottom of stack                              */
        LDDSP   R12, SP[1 * 4]                                          /* Retrieve PC from stack                                   */
        STDSP   SP[7 * 4], R12                                          /* Store PC at bottom of stack                              */
        LDDSP   R12, SP[3 * 4]                                          /* Retrieve R12 back from stack                             */
        SUB     SP, -6 * 4                                              /* Adjust SP to match exception stack frame                 */
        RETE                                                            /* Return to exception handler                              */

        ENDMOD



        MODULE  UNHANDLED_EXCEPTION
        RSEG    CODE32:CODE

        PUBWEAK __unhandled_exception                                   /* weak definition of __unhandled_exception in case not     */
                                                                        /* ... defined anywhere else                                */
__unhandled_exception:
        RJMP    __unhandled_exception
        POPM    R11
        RETE

        ENDMOD
        END
