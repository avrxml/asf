/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                 ATMEL  AVR32 UC3  Board Support Package
*
*                                 (c) Copyright 2007; Micrium; Weston, FL
*                                           All Rights Reserved
*
* File    : BSP.C
* By      : Fabiano Kovalski
*********************************************************************************************************
*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <includes.h>
#include "led.h"
/*
**************************************************************************************************************
*                                         OSCILLATORS FREQUENCY
**************************************************************************************************************
*/

#define     FOSC0                        (12000000)                     /* Frequency: Hz                                            */
#define     RCOSC                       AVR32_PM_RCOSC_FREQUENCY        /* Internal RC oscillator frequency                         */

/*
**************************************************************************************************************
*                                         LED DEFINES
**************************************************************************************************************
*/

#define     LED_COUNT                            4

#define     LED1_GPIO                   AVR32_PIN_PX16
#define     LED2_GPIO                   AVR32_PIN_PX50
#define     LED3_GPIO                   AVR32_PIN_PX54
#define     LED4_GPIO                   AVR32_PIN_PX51                                              */

/*
**************************************************************************************************************
*                                        USART DEFINES
*
* Note(s): Select USART characteristics for engineer or production revision.
**************************************************************************************************************
*/

#define     BSP_USART0_RXD_PIN          AVR32_USART0_RXD_0_0_PIN
#define     BSP_USART0_RXD_FUNCTION     AVR32_USART0_RXD_0_0_FUNCTION
#define     BSP_USART0_TXD_PIN          AVR32_USART0_TXD_0_0_PIN
#define     BSP_USART0_TXD_FUNCTION     AVR32_USART0_TXD_0_0_FUNCTION
#define     BSP_USART1_RXD_PIN          AVR32_USART1_RXD_0_0_PIN
#define     BSP_USART1_RXD_FUNCTION     AVR32_USART1_RXD_0_0_FUNCTION
#define     BSP_USART1_TXD_PIN          AVR32_USART1_TXD_0_0_PIN
#define     BSP_USART1_TXD_FUNCTION     AVR32_USART1_TXD_0_0_FUNCTION
#define     BSP_USART2_RXD_PIN          AVR32_USART2_RXD_0_0_PIN
#define     BSP_USART2_RXD_FUNCTION     AVR32_USART2_RXD_0_0_FUNCTION
#define     BSP_USART2_TXD_PIN          AVR32_USART2_TXD_0_0_PIN
#define     BSP_USART2_TXD_FUNCTION     AVR32_USART2_TXD_0_0_FUNCTION
#define     BSP_USART3_RXD_PIN          AVR32_USART3_RXD_0_0_PIN
#define     BSP_USART3_RXD_FUNCTION     AVR32_USART3_RXD_0_0_FUNCTION
#define     BSP_USART3_TXD_PIN          AVR32_USART3_TXD_0_0_PIN
#define     BSP_USART3_TXD_FUNCTION     AVR32_USART3_TXD_0_0_FUNCTION


/*
**************************************************************************************************************
*                                         INTERRUPT REQUESTS
**************************************************************************************************************
*/

#define     BSP_IRQ_COMPARE             AVR32_CORE_COMPARE_IRQ
#if OS_VIEW_MODULE > 0                                                  /* Do not compile this section if OS-View is disabled       */
#if OS_VIEW_COMM_SEL == OS_VIEW_UART_0
  #define   BSP_IRQ_USART               AVR32_USART0_IRQ
#endif
#if OS_VIEW_COMM_SEL == OS_VIEW_UART_1
  #define   BSP_IRQ_USART               AVR32_USART1_IRQ
#endif
#if OS_VIEW_COMM_SEL == OS_VIEW_UART_2
  #define   BSP_IRQ_USART               AVR32_USART2_IRQ
#endif
#if OS_VIEW_COMM_SEL == OS_VIEW_UART_3
  #define   BSP_IRQ_USART               AVR32_USART3_IRQ
#endif
#endif                                                                  /* #if OS_VIEW_MODULE > 0                                   */

#if uC_PROBE_COM_MODULE > 0                                             /* Do not compile this section if Probe is disabled         */
#if PROBE_RS232_COMM_SEL == PROBE_RS232_UART_0
  #define   BSP_IRQ_USART               AVR32_USART0_IRQ
#endif
#if PROBE_RS232_COMM_SEL == PROBE_RS232_UART_1
  #define   BSP_IRQ_USART               AVR32_USART1_IRQ
#endif
#if PROBE_RS232_COMM_SEL == PROBE_RS232_UART_2
  #define   BSP_IRQ_USART               AVR32_USART2_IRQ
#endif
#if PROBE_RS232_COMM_SEL == PROBE_RS232_UART_3
  #define   BSP_IRQ_USART               AVR32_USART3_IRQ
#endif
#endif                                                                  /* #if uC_PROBE_COM_MODULE > 0                              */

/*
**************************************************************************************************************
*                                         INTERRUPT CONTROLLER
**************************************************************************************************************
*/

#define     BSP_INTC_IRQS_PER_GRP               32                      /* Maximum number of interrupt request lines per group      */

#define     BSP_INTC_IPR_INTLEVEL_OFFSET        30                      /* Interrupt level offset in Interrupt Priority Register    */

#define     BSP_INTC_INT0                        0                      /* lowest interrupt priority level                          */
#define     BSP_INTC_INT1                        1
#define     BSP_INTC_INT2                        2
#define     BSP_INTC_INT3                        3                      /* highest interrupt priority level                         */

#define     AVR32_INTC_ICR_reg            ((volatile CPU_INT32U *)(AVR32_INTC_ADDRESS + 0x200))


/*$PAGE*/
/*
**********************************************************************************************************
*                                           DATA TYPES
**********************************************************************************************************
*/

typedef struct bsp_intc_handler                                         /* hold number of irqs & user-vectors for interrupt groups  */
{
              CPU_INT32U     num_irqs;
    volatile  CPU_FNCT_VOID *handlers;
} BSP_INTC_HANDLER;

/*
**********************************************************************************************************
*                                           VARIABLES
**********************************************************************************************************
*/
                                                                        /* table of interrupt handlers for each interrupt group     */
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_0 [AVR32_INTC_NUM_IRQS_PER_GRP0];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_1 [AVR32_INTC_NUM_IRQS_PER_GRP1];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_2 [AVR32_INTC_NUM_IRQS_PER_GRP2];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_3 [AVR32_INTC_NUM_IRQS_PER_GRP3];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_4 [AVR32_INTC_NUM_IRQS_PER_GRP4];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_5 [AVR32_INTC_NUM_IRQS_PER_GRP5];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_6 [AVR32_INTC_NUM_IRQS_PER_GRP6];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_7 [AVR32_INTC_NUM_IRQS_PER_GRP7];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_8 [AVR32_INTC_NUM_IRQS_PER_GRP8];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_9 [AVR32_INTC_NUM_IRQS_PER_GRP9];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_10[AVR32_INTC_NUM_IRQS_PER_GRP10];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_11[AVR32_INTC_NUM_IRQS_PER_GRP11];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_12[AVR32_INTC_NUM_IRQS_PER_GRP12];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_13[AVR32_INTC_NUM_IRQS_PER_GRP13];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_14[AVR32_INTC_NUM_IRQS_PER_GRP14];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_15[AVR32_INTC_NUM_IRQS_PER_GRP15];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_16[AVR32_INTC_NUM_IRQS_PER_GRP16];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_17[AVR32_INTC_NUM_IRQS_PER_GRP17];
static  volatile  CPU_FNCT_VOID  Int_Handler_Tbl_18[AVR32_INTC_NUM_IRQS_PER_GRP18];

/*
**********************************************************************************************************
*                                           TABLES
**********************************************************************************************************
*/

const  BSP_INTC_HANDLER  BSP_INTC_Handlers[AVR32_INTC_NUM_INT_GRPS] = { /* table of all int groups, with max irq number & user vect */
    {AVR32_INTC_NUM_IRQS_PER_GRP0,  Int_Handler_Tbl_0},
    {AVR32_INTC_NUM_IRQS_PER_GRP1,  Int_Handler_Tbl_1},
    {AVR32_INTC_NUM_IRQS_PER_GRP2,  Int_Handler_Tbl_2},
    {AVR32_INTC_NUM_IRQS_PER_GRP3,  Int_Handler_Tbl_3},
    {AVR32_INTC_NUM_IRQS_PER_GRP4,  Int_Handler_Tbl_4},
    {AVR32_INTC_NUM_IRQS_PER_GRP5,  Int_Handler_Tbl_5},
    {AVR32_INTC_NUM_IRQS_PER_GRP6,  Int_Handler_Tbl_6},
    {AVR32_INTC_NUM_IRQS_PER_GRP7,  Int_Handler_Tbl_7},
    {AVR32_INTC_NUM_IRQS_PER_GRP8,  Int_Handler_Tbl_8},
    {AVR32_INTC_NUM_IRQS_PER_GRP9,  Int_Handler_Tbl_9},
    {AVR32_INTC_NUM_IRQS_PER_GRP10, Int_Handler_Tbl_10},
    {AVR32_INTC_NUM_IRQS_PER_GRP11, Int_Handler_Tbl_11},
    {AVR32_INTC_NUM_IRQS_PER_GRP12, Int_Handler_Tbl_12},
    {AVR32_INTC_NUM_IRQS_PER_GRP13, Int_Handler_Tbl_13},
    {AVR32_INTC_NUM_IRQS_PER_GRP14, Int_Handler_Tbl_14},
    {AVR32_INTC_NUM_IRQS_PER_GRP15, Int_Handler_Tbl_15},
    {AVR32_INTC_NUM_IRQS_PER_GRP16, Int_Handler_Tbl_16},
    {AVR32_INTC_NUM_IRQS_PER_GRP17, Int_Handler_Tbl_17},
    {AVR32_INTC_NUM_IRQS_PER_GRP18, Int_Handler_Tbl_18},
};

/*$PAGE*/
/*
*********************************************************************************************************
*                                         LOCAL PROTOTYPES
*********************************************************************************************************
*/

static  void    BSP_PM_OSC0Setup(void);
static  void    BSP_PM_PLL0Setup(CPU_INT08U mul, CPU_INT08U div, CPU_BOOLEAN div2);
static  void    BSP_PM_GClkSetup(CPU_INT08U pll);
static  void    BSP_PM_ClkSelect(CPU_BOOLEAN cpudiv, CPU_INT08U cpusel,
                                 CPU_BOOLEAN pbadiv, CPU_INT08U pbasel,
                                 CPU_BOOLEAN pbbdiv, CPU_INT08U pbbsel);
static  void    BSP_PM_ClkSwitch(CPU_INT08U clk);

static  void    BSP_INTC_Init(void);

static  void    BSP_FLASHC_SetWaitState(CPU_INT16U ws);

static  void    BSP_TmrInit(void);
static  void    BSP_TmrReload(void);
static  void    BSP_TmrTickISR(void);

static  void    LED_Init(void);

#if ((OS_VIEW_MODULE > 0) || (uC_PROBE_COM_MODULE > 0))                 /* Do not compile this section                              */
                                                                        /* ...  if OS-View and Probe are disabled                   */
static  void    BSP_USARTRxTxISR (void);
#endif                                                                  /* #if (OS_VIEW_MODULE > 0) || (uC_PROBE_COM_MODULE > 0)    */


/*
*********************************************************************************************************
*                                         BSP INITIALIZATION
*
* Description: This function should be called by the application code before using any functions in this module.
*
* Arguments  : none
*
* Note(s)    : To run the EVK1100 at clock rate of:
*                   48MHz: call BSP_PM_PLL0Setup( 8, 1, 1),
*                   54MHz: call BSP_PM_PLL0Setup( 9, 1, 1)
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    LED_Init();

    BSP_INTC_Init();
    BSP_INTC_IntReg(&BSP_TmrTickISR, BSP_IRQ_COMPARE, 3);
#if ((OS_VIEW_MODULE > 0) || (uC_PROBE_COM_MODULE > 0))                 /* Do not compile this section                              */
                                                                        /* ...  if OS-View and Probe are disabled                   */
    BSP_INTC_IntReg(&BSP_USARTRxTxISR, BSP_IRQ_USART, 1);
#endif                                                                  /* #if (OS_VIEW_MODULE > 0) || (uC_PROBE_COM_MODULE > 0)    */

    BSP_TmrInit();                                                      /* Initialize OS periodical time source                     */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP, POWER MANAGER - OSC0 SETUP
*
* Description: This function configures oscillator 0.
*
* Arguments  : none
*********************************************************************************************************
*/

static  void  BSP_PM_OSC0Setup (void)
{
    CPU_INT32U  osc0;


    osc0               =  0;

    if (FOSC0 < 8000000) {
        osc0          |=  0x06 << AVR32_PM_OSCCTRL0_MODE_OFFSET;        /* AVR32_PM_OSCCTRL0_MODE_CRYSTAL_G2                        */
    } else {
        osc0          |=  0x07 << AVR32_PM_OSCCTRL0_MODE_OFFSET;        /* AVR32_PM_OSCCTRL0_MODE_CRYSTAL_G3 or MODE_CRYSTAL        */
    }

    osc0              |=  7 << AVR32_PM_OSCCTRL0_STARTUP_OFFSET;        /* Start-up time: RCOsc periods                             */

    AVR32_PM.oscctrl0  =  osc0;


    AVR32_PM.mcctrl   |=  AVR32_PM_MCCTRL_OSC0EN_MASK;                  /* Load current value of MCCTRL register                    */

    while (AVR32_PM.POSCSR.osc0rdy == 0) {
        ;                                                               /* Wait until OSC0 is ready                                 */
    }
}

/*
*********************************************************************************************************
*                                         BSP, POWER MANAGER - PLL0 SETUP
*
* Description: This function configures PLL0 to run with oscillator 0 as a frequency source.
*
* Arguments  : mul   4-bit multiply factor
*              div   4-bit division factor
*              div2  output frequency divide by 2 option
*
* Note(s)    : 1) fPLL = fOSC * mul / div
*
*              2) For 132MHz PLL, use mul=11, div=1. Then set div2=1 to have Fout as 66MHz.
*                 For  96MHz PLL, use mul= 8, div=1. Then set div2=1 to have Fout as 48MHz.
*********************************************************************************************************
*/

static  void  BSP_PM_PLL0Setup (CPU_INT08U mul, CPU_INT08U div, CPU_BOOLEAN div2)
{
    CPU_INT32U  pll;
    CPU_INT32U  opt;


    pll              =   0;
    pll             |= ((mul - 1) << AVR32_PM_PLL0_PLLMUL_OFFSET)   &
                        AVR32_PM_PLL0_PLLMUL_MASK;
    pll             |=  (div      << AVR32_PM_PLL0_PLLDIV_OFFSET)   &
                        AVR32_PM_PLL0_PLLDIV_MASK;
    pll             |=  (0        << AVR32_PM_PLL0_PLLOSC_OFFSET)   &   /* Select OSC0                                              */
                        AVR32_PM_PLL0_PLLOSC_MASK;
    pll             |=  (16       << AVR32_PM_PLL0_PLLCOUNT_OFFSET) &   /* Slow clk cycles before ISR:LOCK is set, ~500us           */
                        AVR32_PM_PLL0_PLLCOUNT_MASK;

    if ((mul * FOSC0 / div) >= 160000000) {
        opt          =  (0    << 0) |                                   /* VCO Frequency: {0} 160MHz-240MHz                         */
                        (div2 << 1) |                                   /* Output Divider: {0} Fout = Fvco; {1} Fout = Fvco/2       */
                        (0    << 2);                                    /* Wide Bandwidth Mode: {0} enable; {1} disable             */
    } else {
        opt          =  (1    << 0) |                                   /* VCO Frequency: {1} 80MHZ-160MHz                          */
                        (div2 << 1) |                                   /* Output Divider: {0} Fout = Fvco; {1} Fout = Fvco/2       */
                        (0    << 2);                                    /* Wide Bandwidth Mode: {0} enable; {1} disable             */
    }

    pll             |=  (opt      << AVR32_PM_PLL0_PLLOPT_OFFSET)   &
                        AVR32_PM_PLL0_PLLOPT_MASK;
    pll             &= ~AVR32_PM_PLL0_PLLEN_MASK;                       /* Disable PLL to store the changes                         */

    AVR32_PM.pll[0]  =  pll;                                            /* Load new PLL settings                                    */

    pll             |=  AVR32_PM_PLL0_PLLEN_MASK;                       /* Enable PLL                                               */
    AVR32_PM.pll[0]  =  pll;

    while (AVR32_PM.POSCSR.lock0 == 0) {
        ;                                                               /* Wait until PLL0 is ready                                 */
    }

    AVR32_PM.pll[0] |=  AVR32_PM_PLL0_PLLBPL_MASK;                      /* Bypass the lock signal of the PLL                        */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP, POWER MANAGER - GENERIC CLK SETUP
*
* Description: This function configures generic clock 0 to run with oscillator or PLL 0 as a frequency source.
*
* Arguments  : pll   enable PLL as a frequency source for the generic clock
*********************************************************************************************************
*/

static  void  BSP_PM_GClkSetup (CPU_BOOLEAN pll)
{
    CPU_INT32U  gclk;


    gclk                =  0;

    gclk               |= (0   << AVR32_PM_GCCTRL_OSCSEL_OFFSET) &      /* Generic Clock source: OSC0/PLL0 or OSC1/PLL1             */
                           AVR32_PM_GCCTRL_OSCSEL_MASK;
    gclk               |= (pll << AVR32_PM_GCCTRL_PLLSEL_OFFSET) &      /* PLL Select: OSC (=0) or PLL (=1)                         */
                           AVR32_PM_GCCTRL_PLLSEL_MASK;
    gclk               |= (0   << AVR32_PM_GCCTRL_DIVEN_OFFSET)  &      /* Divide Enable                                            */
                           AVR32_PM_GCCTRL_DIVEN_MASK;
    gclk               |= (0   << AVR32_PM_GCCTRL_DIV_OFFSET)    &      /* Division factor: 8-bits                                  */
                           AVR32_PM_GCCTRL_DIV_MASK;

    gclk               &= ~AVR32_PM_GCCTRL_CEN_MASK;                    /* Disable Generic Clock 0 to store changes                 */
    AVR32_PM.gcctrl[0]  =  gclk;                                        /* Load new Generic Clock 0 settings                        */

    gclk               |=  AVR32_PM_GCCTRL_CEN_MASK;                    /* Enable Generic Clock 0                                   */
    AVR32_PM.gcctrl[0]  =  gclk;

    BSP_GPIO_SetFnct(AVR32_PM_GCLK_0_1_PIN,                             /* Assign GPIO GCLK_0_1 pin output Generic Clock 0          */
                     AVR32_PM_GCLK_0_1_FUNCTION);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP, POWER MANAGER - CLK SELECT
*
* Description: This function configures the synchronous clocks for CPU/HSB, PBA, and PBB modules.
*
* Arguments  : cpudiv   cpu/hsb clock division enable
*              cpusel   cpu/sel division option (3-bits)
*              pbadiv   pba     clock division enable
*              pbasel   pba     division option (3-bits)
*              pbbdiv   pbb     clock division enable
*              pbbsel   pbb     division option (3-bits)
*
* Note(s)    : 1) If xxxDIV is written to 0, xxxSEL should also be written to 0 to ensure correct operation
*
*              2) Main clock is divided by 2^(xxxSEL + 1)
*
*              3) In AT32UC3, HSB shares the source clock with the CPU, thus writing to HSBDIV & HSBSEL
*                 has no effect.
*
*              4) The synchronous clock source can be changed on-the-fly.
*
*              5) writing a previous value to xxxDIV and xxxSEL keeps the clock unchanged without halting
*                 or disabling the module.
*
*              6) Scaling CPU/HSB speed can be implemented according to the required performance, while
*                 keeping PBA and PBB frequency constant
*********************************************************************************************************
*/

static  void  BSP_PM_ClkSelect (CPU_BOOLEAN cpudiv, CPU_INT08U cpusel,
                                CPU_BOOLEAN pbadiv, CPU_INT08U pbasel,
                                CPU_BOOLEAN pbbdiv, CPU_INT08U pbbsel)
{
    CPU_INT32U  clksel;


    clksel          =  0;

    clksel         |= (cpudiv << AVR32_PM_CKSEL_CPUDIV_OFFSET) &        /* CPU & HSB are equal on AT32UC3A                          */
                       AVR32_PM_CKSEL_CPUDIV_MASK;
    clksel         |= (cpusel << AVR32_PM_CKSEL_CPUSEL_OFFSET) &        /* CPU & HSB are equal on AT32UC3A                          */
                       AVR32_PM_CKSEL_CPUSEL_MASK;
    clksel         |= (cpudiv << AVR32_PM_CKSEL_HSBDIV_OFFSET) &        /* HSB Division enable                                      */
                       AVR32_PM_CKSEL_HSBDIV_MASK;
    clksel         |= (cpusel << AVR32_PM_CKSEL_HSBSEL_OFFSET) &        /* HSB Clock select: clk divided by 2^(SEL+1)  SEL[0:2]     */
                       AVR32_PM_CKSEL_HSBSEL_MASK;
    clksel         |= (pbadiv << AVR32_PM_CKSEL_PBADIV_OFFSET) &
                       AVR32_PM_CKSEL_PBADIV_MASK;
    clksel         |= (pbasel << AVR32_PM_CKSEL_PBASEL_OFFSET) &
                       AVR32_PM_CKSEL_PBASEL_MASK;
    clksel         |= (pbbdiv << AVR32_PM_CKSEL_PBBDIV_OFFSET) &
                       AVR32_PM_CKSEL_PBBDIV_MASK;
    clksel         |= (pbbsel << AVR32_PM_CKSEL_PBBSEL_OFFSET) &
                       AVR32_PM_CKSEL_PBBSEL_MASK;

    AVR32_PM.cksel  =  clksel;

    while (AVR32_PM.POSCSR.ckrdy == 0) {
        ;                                                               /* Wait until new clock settings become effective           */
    }
}


/*
*********************************************************************************************************
*                                         BSP, POWER MANAGER - CLK SWITCH
*
* Description: This function switches the main clock.
*
* Arguments  : clk      main clock selection {AVR32_PM_MCSEL_PLL0, AVR32_PM_MCSEL_OSC0, AVR32_PM_MCSEL_SLOW}
*
* Note(s)    : 1) The clock source has to be enable and ready, otherwise a deadlock will occur.
*********************************************************************************************************
*/

static  void  BSP_PM_ClkSwitch (CPU_INT08U clk)
{
    CPU_INT32U  mcctrl;


    mcctrl          =  AVR32_PM.mcctrl;                                 /* Load current value of MCCTRL register                    */
    mcctrl         &= ~AVR32_PM_MCCTRL_MCSEL_MASK;                      /* Clear MCSEL options                                      */
    mcctrl         |= (clk << AVR32_PM_MCCTRL_MCSEL_OFFSET) &           /* Select main clock source                                 */
                       AVR32_PM_MCCTRL_MCSEL_MASK;

    AVR32_PM.mcctrl =  mcctrl;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP INTC, DEFAULT INTERRUPT HANDLER
*
* Description: This function catches all unhandled interrupts.
*
* Arguments  : none
*********************************************************************************************************
*/

static  void  BSP_INTC_IntUnhandled (void)
{
    while (1) {
        ;                                                               /* Catch all unregistered interrupts                        */
    }
}


/*
*********************************************************************************************************
*                                         BSP INTC, GET INTERRUPT HANDLER
*
* Description: This function retrieves the interrupt handler associated with the current interrupt.
*
* Arguments  : int_level    current interrupt priority level
*
* Return     : User-defined interrupt service function
*
* Note(s)    : 1) The proper way to clear or disable peripheral interrupts should be done with the following sequence:
*                   a) Mask the interrupt in the CPU by setting GM (or IxM) in SR,
*                   b) Perform the bus access to the peripheral register that clears or disables the interrupt,
*                   c) Wait until the interrupt has actually been cleared or disabled by the peripheral. This is usually
*                      performed by reading from a register in the same peripheral (it DOES NOT have to be the same
*                      register that was accessed in step 'b', but it MUST be in the same peripheral). This procedure
*                      takes bus system latencies into account, but peripheral internal latencies (generally 0 cycle)
*                      also have to be considered.
*                   d) Unmask the interrupt in the CPU by clearing GM (or IxM) in SR.
*
*              2) When multiple interrupt lines are active, the highest line is selected. This preserves the prioritization
*                 of the interrupt groups performed by the interrupt controller.
*
*              3) BSP_INTC_Init() must have been called prior to enable global interrupts, otherwise any unhandled interrupt
*                 will generate an unexpected behavior.
*
*              4) The user-defined interrupt function must be registered calling BSP_INTC_IntReg().
*
*              5) Global interrupts MUST be disabled before calling this function.
*********************************************************************************************************
*/

CPU_FNCT_VOID  BSP_INTC_IntGetHandler (CPU_INT32U int_level)
{
    CPU_INT32U  int_grp;
    CPU_INT32U  int_req;
    CPU_INT32U  int_id;


    int_grp = AVR32_INTC_ICR_reg[BSP_INTC_INT3 - int_level];            /* icr3 is memory mapped first, icr0 last                   */
    int_req = AVR32_INTC.irr[int_grp];
    int_id  = 32 - CPU_CntLeadZeros(int_req) - 1;                       /* select highest int line when multiple lines are active   */

    if (int_req == 0 ) {
        return (0);
    } else {
        return (BSP_INTC_Handlers[int_grp].handlers[int_id]);
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP INTC, INITIALIZE INTERRUPTS
*
* Description: This function initializes the interrupt controller with all int groups using priority level 0 and
*              with the unhandled interrupt user-handler.
*
* Arguments  : none
*********************************************************************************************************
*/

static  void  BSP_INTC_Init (void)
{
    CPU_INT32U  int_grp;
    CPU_INT32U  int_req;


    for (int_grp = 0; int_grp < AVR32_INTC_NUM_INT_GRPS; int_grp++) {   /* loop through all interrupt groups                        */
                                                                        /* loop through each interrupt request line inside a group  */
        for (int_req = 0; int_req < BSP_INTC_Handlers[int_grp].num_irqs; int_req++) {
                                                                        /* assign BSP_INTC_IntUnhandled() as default int handler    */
            BSP_INTC_Handlers[int_grp].handlers[int_req] = &BSP_INTC_IntUnhandled;
        }

        AVR32_INTC.ipr[int_grp] =  OSIntPrioReg[BSP_INTC_INT0] |        /* set all IPR to interrupt priority level 0                */
                                  (BSP_INTC_INT0 << BSP_INTC_IPR_INTLEVEL_OFFSET);
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP INTC, REGISTER USER INTERRUPT FUNCTION
*
* Description: This function registers an user-defined interrupt function to be called by the interrupt handler.
*
* Arguments  : handler      user-defined function, prototyped as: void fnctName (void)
*
*              irq          interrupt number to register
*
*              int_level    interrupt priority level to register
*
* Return     : BSP_INTC_ERR_NONE            if the function was successful
*              BSP_INTC_ERR_INVALID_IRQ     if irq could not be registered, Note #1.
*
* Note(s)    : 1) argument 'irq' is the interrupt request number and not the interrupt group number.
*
*              2) interrupt requests for the same group will have the same priority level as the latest registered interrupt.
*********************************************************************************************************
*/

CPU_INT32U  BSP_INTC_IntReg (CPU_FNCT_VOID handler, CPU_INT32U irq, CPU_INT32U int_level)
{
    CPU_INT32U  int_grp;
    CPU_INT32U  int_id;


    int_grp = irq / BSP_INTC_IRQS_PER_GRP;
    int_id  = irq % BSP_INTC_IRQS_PER_GRP;

    if (int_id > BSP_INTC_Handlers[int_grp].num_irqs) {                 /* test if int_id is defined in BSP_INTC_Handlers[int_grp]  */
        return (BSP_INTC_ERR_INVALID_IRQ);
    }

    BSP_INTC_Handlers[int_grp].handlers[int_id] = handler;              /* store user-handler to be retrieved at interrupt call     */
                                                                        /* program IPR with selected interrupt priority level       */
    AVR32_INTC.ipr[int_grp]                     = OSIntPrioReg[int_level & 0x3] |
                                                  ((int_level & 0x3) << BSP_INTC_IPR_INTLEVEL_OFFSET);

    return (BSP_INTC_ERR_NONE);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP INTC, REGISTER FAST INTERRUPT FUNCTION
*
* Description: This function registers an fast (non-OS) interrupt function to be called by the interrupt handler.
*
* Arguments  : handler      user-defined function, prototyped as: void fnctName (void)
*
*              irq          interrupt number to register
*
*              int_level    interrupt priority level to register
*
* Return     : BSP_INTC_ERR_NONE            if the function was successful
*              BSP_INTC_ERR_INVALID_IRQ     if irq could not be registered, Note #1.
*
* Note(s)    : 1) argument 'irq' is the interrupt request number and not the interrupt group number.
*
*              2) interrupt requests for the same group will have the same priority level as the latest registered interrupt.
*********************************************************************************************************
*/

CPU_INT32U  BSP_INTC_FastIntReg (CPU_FNCT_VOID handler, CPU_INT32U irq, CPU_INT32U int_level)
{
    CPU_INT32U  int_grp;
    CPU_INT32U  int_id;


    int_grp = irq / BSP_INTC_IRQS_PER_GRP;
    int_id  = irq % BSP_INTC_IRQS_PER_GRP;

    if (int_id > BSP_INTC_Handlers[int_grp].num_irqs) {                 /* test if int_id is defined in BSP_INTC_Handlers[int_grp]  */
        return (BSP_INTC_ERR_INVALID_IRQ);
    }

    BSP_INTC_Handlers[int_grp].handlers[int_id] = handler;              /* store user-handler to be retrieved at interrupt call     */
                                                                        /* program IPR with selected interrupt priority level       */
    AVR32_INTC.ipr[int_grp]                     = OSIntPrioReg[4 + (int_level & 0x3)] |
                                                  ((int_level & 0x3) << BSP_INTC_IPR_INTLEVEL_OFFSET);

    return (BSP_INTC_ERR_NONE);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP, FLASH CONTROLLER - SET WAIT STATE
*
* Description: This function configures the flash wait states.
*
* Arguments  : ws       flash wait states {0, 1}
*
* Note(s)    : 1) 0 WS access is up to 30MHz CPU/HSB clock
*********************************************************************************************************
*/

static  void  BSP_FLASHC_SetWaitState (CPU_INT16U ws)
{
    if (ws == 0) {
        AVR32_FLASHC.fcr &= ~AVR32_FLASHC_FWS_MASK;
    }
    if (ws == 1) {
        AVR32_FLASHC.fcr |=  AVR32_FLASHC_FWS_MASK;
    }
}


/*
*********************************************************************************************************
*                                         BSP, GPIO - SET INTERRUPT MODE
*
* Description: This function configures the mode of interrupt of a pin.
*
* Arguments  : pin      port pin (GPIO number)
*              mode     selected interrupt mode { Pin Change, Rising Edge, or Falling Edge }
*
* Note(s)    : The PA, PB, PC and PX ports do not directly correspond to the GPIO ports. The relationship is given by:
*                   GPIO port = floor((GPIO number) / 32)
*                   GPIO pin  = GPIO number mod 32
*********************************************************************************************************
*/

void  BSP_GPIO_SetIntMode (CPU_INT16U pin, CPU_INT08U mode)
{
    volatile  avr32_gpio_port_t  *gpio_port;


    gpio_port                  = &AVR32_GPIO.port[pin / 32];

    gpio_port->gpers           = 1 << (pin % 32);                       /* GPIO controller enable                                   */
    gpio_port->gfers           = 1 << (pin % 32);                       /* GPIO glitch filter enable                                */

    switch (mode) {
        case BSP_INT_PIN_CHANGE:
             gpio_port->imr0c  = 1 << (pin % 32);
             gpio_port->imr1c  = 1 << (pin % 32);
             break;

        case BSP_INT_RISING_EDGE:
             gpio_port->imr0s  = 1 << (pin % 32);
             gpio_port->imr1c  = 1 << (pin % 32);
             break;

        case BSP_INT_FALLING_EDGE:
             gpio_port->imr0c  = 1 << (pin % 32);
             gpio_port->imr1s  = 1 << (pin % 32);
             break;
    }

    gpio_port->iers            = 1 << (pin % 32);                       /* GPIO interrupt enable                                    */
}


/*
*********************************************************************************************************
*                                         BSP, GPIO - READ INTERRUPT STATE
*
* Description: This function reads the interrupt flag state of a pin.
*
* Arguments  : pin      port pin (GPIO number)
*
* Note(s)    : The PA, PB, PC and PX ports do not directly correspond to the GPIO ports. The relationship is given by:
*                   GPIO port = floor((GPIO number) / 32)
*                   GPIO pin  = GPIO number mod 32
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_GPIO_IntFlagRd (CPU_INT16U pin)
{
    volatile  avr32_gpio_port_t  *gpio_port;
              CPU_INT32U          int_mask;


    gpio_port                  = &AVR32_GPIO.port[pin / 32];

    int_mask                   = 1 << (pin % 32);

    if ((gpio_port->ifr & int_mask) == int_mask) {
        return (1);
    } else {
        return (0);
    }
}


/*
*********************************************************************************************************
*                                         BSP, GPIO - CLEAR INTERRUPT STATE
*
* Description: This function clears the interrupt flag state of a pin.
*
* Arguments  : pin      port pin (GPIO number)
*
* Note(s)    : The PA, PB, PC and PX ports do not directly correspond to the GPIO ports. The relationship is given by:
*                   GPIO port = floor((GPIO number) / 32)
*                   GPIO pin  = GPIO number mod 32
*********************************************************************************************************
*/

void  BSP_GPIO_IntFlagClr (CPU_INT16U pin)
{
    volatile  avr32_gpio_port_t  *gpio_port;


    gpio_port                  = &AVR32_GPIO.port[pin / 32];

    gpio_port->ifrc            = 1 << (pin % 32);
}


/*
*********************************************************************************************************
*                                         BSP, GPIO - SET PERIPHERAL FUNCTION
*
* Description: This function configures the peripheral function of a pin.
*
* Arguments  : pin      port pin (GPIO number)
*              fnct     selected peripheral function
*
* Note(s)    : The PA, PB, PC and PX ports do not directly correspond to the GPIO ports. The relationship is given by:
*                   GPIO port = floor((GPIO number) / 32)
*                   GPIO pin  = GPIO number mod 32
*********************************************************************************************************
*/

void  BSP_GPIO_SetFnct (CPU_INT16U pin, CPU_INT08U fnct)
{
    volatile  avr32_gpio_port_t  *gpio_port;


    gpio_port        = &AVR32_GPIO.port[pin / 32];

    switch (fnct) {
        case 0:                                                         /* A function                                               */
             gpio_port->pmr0c = 1 << (pin % 32);                        /* clear Peripheral Mux Register 0                          */
             gpio_port->pmr1c = 1 << (pin % 32);                        /* clear Peripheral Mux Register 1                          */
             break;

        case 1:                                                         /* B function                                               */
             gpio_port->pmr0s = 1 << (pin % 32);                        /* set   Peripheral Mux Register 0                          */
             gpio_port->pmr1c = 1 << (pin % 32);                        /* clear Peripheral Mux Register 1                          */
             break;

        case 2:                                                         /* C function                                               */
             gpio_port->pmr0c = 1 << (pin % 32);                        /* clear Peripheral Mux Register 0                          */
             gpio_port->pmr1s = 1 << (pin % 32);                        /* set   Peripheral Mux Register 1                          */
             break;

        case 3:                                                         /* D function                                               */
             gpio_port->pmr0s = 1 << (pin % 32);                        /* set   Peripheral Mux Register 0                          */
             gpio_port->pmr1s = 1 << (pin % 32);                        /* set   Peripheral Mux Register 1                          */
             break;
    }

    gpio_port->gperc = 1 << (pin % 32);                                 /* disable GPIO control, pin controlled by the peripheral   */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP, GPIO - GET PIN STATE
*
* Description: This function retrieves the level of a pin.
*
* Arguments  : pin      port pin (GPIO number)
*
* Return     : true is returned if the pin level is set to high.
*
* Note(s)    : The PA, PB, PC and PX ports do not directly correspond to the GPIO ports. The relationship is given by:
*                   GPIO port = floor((GPIO number) / 32)
*                   GPIO pin  = GPIO number mod 32
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_GPIO_PinGet (CPU_INT16U pin)
{
    volatile  avr32_gpio_port_t  *gpio_port;
              CPU_INT32U          pvr;


    gpio_port        = &AVR32_GPIO.port[pin / 32];

    pvr              =  gpio_port->pvr;                                 /* Retrieve pin level for selected port                     */
    pvr            >>=  pin % 32;
    pvr             &=  1;

    return (pvr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP, GPIO - SET PIN
*
* Description: This function sets a pin level to high.
*
* Arguments  : pin      port pin (GPIO number)
*
* Note(s)    : The PA, PB, PC and PX ports do not directly correspond to the GPIO ports. The relationship is given by:
*                   GPIO port = floor((GPIO number) / 32)
*                   GPIO pin  = GPIO number mod 32
*********************************************************************************************************
*/

void  BSP_GPIO_PinSet (CPU_INT16U pin)
{
    volatile  avr32_gpio_port_t  *gpio_port;


    gpio_port        = &AVR32_GPIO.port[pin / 32];

    gpio_port->ovrs  = 1 << (pin % 32);                                 /* Set high level on the pin                                */
    gpio_port->oders = 1 << (pin % 32);                                 /* Enable GPIO output driver for that pin                   */
    gpio_port->gpers = 1 << (pin % 32);                                 /* Pin controlled by the GPIO                               */
}


/*
*********************************************************************************************************
*                                         BSP, GPIO - CLEAR PIN
*
* Description: This function sets a pin level to low.
*
* Arguments  : pin      port pin (GPIO number)
*
* Note(s)    : The PA, PB, PC and PX ports do not directly correspond to the GPIO ports. The relationship is given by:
*                   GPIO port = floor((GPIO number) / 32)
*                   GPIO pin  = GPIO number mod 32
*********************************************************************************************************
*/

void  BSP_GPIO_PinClr (CPU_INT16U pin)
{
    volatile  avr32_gpio_port_t  *gpio_port;


    gpio_port        = &AVR32_GPIO.port[pin / 32];

    gpio_port->ovrc  = 1 << (pin % 32);                                 /* Set low level on the pin                                 */
    gpio_port->oders = 1 << (pin % 32);                                 /* Enable GPIO output driver for that pin                   */
    gpio_port->gpers = 1 << (pin % 32);                                 /* Pin controlled by the GPIO                               */
}


/*
*********************************************************************************************************
*                                         BSP, GPIO - SET PIN
*
* Description: This function toggles the level on a pin.
*
* Arguments  : pin      port pin (GPIO number)
*
* Note(s)    : The PA, PB, PC and PX ports do not directly correspond to the GPIO ports. The relationship is given by:
*                   GPIO port = floor((GPIO number) / 32)
*                   GPIO pin  = GPIO number mod 32
*********************************************************************************************************
*/

void BSP_GPIO_PinTgl (CPU_INT16U pin)
{
    volatile  avr32_gpio_port_t  *gpio_port;


    gpio_port        = &AVR32_GPIO.port[pin / 32];

    gpio_port->ovrt  = 1 << (pin % 32);                                 /* Toggle level on the pin                                  */
    gpio_port->oders = 1 << (pin % 32);                                 /* Enable GPIO output driver for that pin                   */
    gpio_port->gpers = 1 << (pin % 32);                                 /* Pin controlled by the GPIO                               */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP, CPU - CPU OPERATING FREQUENCY
*
* Description: This function returns the current cpu frequency.
*
* Arguments  : none
*
* Returns    : CPU frequency in clock units per second (Hz)
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    CPU_INT08U  mul;
    CPU_INT08U  div;
    CPU_INT08U  sel;
    CPU_INT32U  clk;


    if (AVR32_PM.CKSEL.cpudiv == 0) {                                   /* Clock Division: main clock divided by 2^(CPUSEL + 1)     */
        sel =  0;
    } else {
        sel =  AVR32_PM.CKSEL.cpusel + 1;
    }

    switch (AVR32_PM.MCCTRL.mcsel) {
        case AVR32_PM_MCSEL_SLOW:
             clk       =  RCOSC >> sel;
             break;

        case AVR32_PM_MCSEL_OSC0:
             clk       =  FOSC0 >> sel;
             break;

        case AVR32_PM_MCSEL_PLL0:
             mul       =  AVR32_PM.PLL[0].pllmul + 1;
             div       =  AVR32_PM.PLL[0].plldiv;

             if (AVR32_PM.PLL[0].pllopt & 0x2) {                        /* Output Divider: {0} Fout = Fvco; {1} Fout = Fvco/2       */
                 div <<=  1;
             }

             clk       =  (mul * FOSC0 / div) >> sel;
             break;

        default:
             clk       =  0;
             break;
    }

    return (clk);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP, PBA - PBA OPERATING FREQUENCY
*
* Description: This function returns the current pba frequency.
*
* Arguments  : none
*
* Returns    : PBA frequency in clock units per second (Hz)
*********************************************************************************************************
*/

CPU_INT32U  BSP_PBA_ClkFreq (void)
{
    CPU_INT08U  mul;
    CPU_INT08U  div;
    CPU_INT08U  sel;
    CPU_INT32U  clk;


    if (AVR32_PM.CKSEL.pbadiv == 0) {                                   /* Clock Division: main clock divided by 2^(PBASEL + 1)     */
        sel =  0;
    } else {
        sel =  AVR32_PM.CKSEL.pbasel + 1;
    }

    switch (AVR32_PM.MCCTRL.mcsel) {
        case AVR32_PM_MCSEL_SLOW:
             clk       =  RCOSC >> sel;
             break;

        case AVR32_PM_MCSEL_OSC0:
             clk       =  FOSC0 >> sel;
             break;

        case AVR32_PM_MCSEL_PLL0:
             mul       =  AVR32_PM.PLL[0].pllmul + 1;
             div       =  AVR32_PM.PLL[0].plldiv;

             if (AVR32_PM.PLL[0].pllopt & 0x2) {                        /* Output Divider: {0} Fout = Fvco; {1} Fout = Fvco/2       */
                 div <<=  1;
             }

             clk       =  (mul * FOSC0 / div) >> sel;
             break;

        default:
             clk       =  0;
             break;
    }

    return (clk);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         BSP, PBB - PBB OPERATING FREQUENCY
*
* Description: This function returns the current pbb frequency.
*
* Arguments  : none
*
* Returns    : PBB frequency in clock units per second (Hz)
*********************************************************************************************************
*/

CPU_INT32U  BSP_PBB_ClkFreq (void)
{
    CPU_INT08U  mul;
    CPU_INT08U  div;
    CPU_INT08U  sel;
    CPU_INT32U  clk;


    if (AVR32_PM.CKSEL.pbbdiv == 0) {                                   /* Clock Division: main clock divided by 2^(PBBSEL + 1)     */
        sel =  0;
    } else {
        sel =  AVR32_PM.CKSEL.pbbsel + 1;
    }

    switch (AVR32_PM.MCCTRL.mcsel) {
        case AVR32_PM_MCSEL_SLOW:
             clk       =  RCOSC >> sel;
             break;

        case AVR32_PM_MCSEL_OSC0:
             clk       =  FOSC0 >> sel;
             break;

        case AVR32_PM_MCSEL_PLL0:
             mul       =  AVR32_PM.PLL[0].pllmul + 1;
             div       =  AVR32_PM.PLL[0].plldiv;

             if (AVR32_PM.PLL[0].pllopt & 0x2) {                        /* Output Divider: {0} Fout = Fvco; {1} Fout = Fvco/2       */
                 div <<=  1;
             }

             clk       =  (mul * FOSC0 / div) >> sel;
             break;

        default:
             clk       =  0;
             break;
    }

    return (clk);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                  BSP, USART - INITIALIZE
*
* Description: This function initializes the COM port.
*
* Arguments  : com          UART port: 0..3
*              baud_rate    com port baudrate in bps.
*
* Note(s)    : It sets up RS232 {baud_rate} 8N1 on UART{com}
*********************************************************************************************************
*/

#if 1 /* OLD */
void  BSP_USART_Init (CPU_INT08U com, CPU_INT32U baud_rate)
{
    volatile  avr32_usart_t  *usart;
              CPU_INT32U      cd;                                       /* Clock divider                                            */
              CPU_INT32U      pba_freq;


    switch (com) {
        case 0:
             BSP_GPIO_SetFnct(BSP_USART0_RXD_PIN, BSP_USART0_RXD_FUNCTION);
             BSP_GPIO_SetFnct(BSP_USART0_TXD_PIN, BSP_USART0_TXD_FUNCTION);

             usart  = &AVR32_USART0;

             break;

        case 1:
             BSP_GPIO_SetFnct(BSP_USART1_RXD_PIN, BSP_USART1_RXD_FUNCTION);
             BSP_GPIO_SetFnct(BSP_USART1_TXD_PIN, BSP_USART1_TXD_FUNCTION);

             usart  = &AVR32_USART1;

             break;

        case 2:
             BSP_GPIO_SetFnct(BSP_USART2_RXD_PIN, BSP_USART2_RXD_FUNCTION);
             BSP_GPIO_SetFnct(BSP_USART2_TXD_PIN, BSP_USART2_TXD_FUNCTION);

             usart  = &AVR32_USART2;

             break;

        case 3:
             BSP_GPIO_SetFnct(BSP_USART3_RXD_PIN, BSP_USART3_RXD_FUNCTION);
             BSP_GPIO_SetFnct(BSP_USART3_TXD_PIN, BSP_USART3_TXD_FUNCTION);

             usart  = &AVR32_USART3;

             break;

        default:
             return;
    }

    //usart->idr      =  0xFFFFFFFF;                                      /* Disable all USART interrupts                             */

    usart->mr       =  0;                                               /* Reset MODE regsiter                                      */
    usart->rtor     =  0;                                               /* Reset Receiver Time-out register                         */
    usart->ttgr     =  0;                                               /* Reset Transmitter Timeguard register                     */

    usart->cr       =  AVR32_USART_CR_RSTRX_MASK   |                    /* Reset RX                                                 */
                       AVR32_USART_CR_RSTTX_MASK   |                    /* Reset TX                                                 */
                       AVR32_USART_CR_RSTSTA_MASK  |                    /* Reset Status Bits                                        */
                       AVR32_USART_CR_RSTIT_MASK   |                    /* Reset Iterations                                         */
                       AVR32_USART_CR_RSTNACK_MASK |                    /* Reset Non Acknowledge                                    */
                       AVR32_USART_CR_DTRDIS_MASK  |                    /* Disable DTR                                              */
                       AVR32_USART_CR_RTSDIS_MASK;                      /* Disable RTS                                              */

    pba_freq        =  BSP_PBA_ClkFreq();                               /* Retrieve Peripheral Bus A operating frequency            */

    if (baud_rate < (pba_freq / 16)) {                                  /* 8x oversampling                                          */
        usart->mr  |=  AVR32_USART_MR_OVER_MASK;                        /* Set oversampling mask                                    */
        cd          =  pba_freq / ( 8 * baud_rate);
    } else {                                                            /* 16x oversampling                                         */
        usart->mr  &= ~AVR32_USART_MR_OVER_MASK;                        /* Clear oversampling mask                                  */
        cd          =  pba_freq / (16 * baud_rate);
    }

    usart->brgr     =  cd << AVR32_USART_BRGR_CD_OFFSET;                /* Set CR on Baud Rate Generator register                   */
                                                                        /* Set normal RS232 mode, no parity bit, 1 stop bit,        */
    usart->mr      |= (AVR32_USART_MR_MODE_NORMAL << AVR32_USART_MR_CHMODE_OFFSET) |
                      (AVR32_USART_MR_PAR_NONE    << AVR32_USART_MR_PAR_OFFSET)    |
                      (AVR32_USART_MR_NBSTOP_1    << AVR32_USART_MR_NBSTOP_OFFSET) |
                      ((8 - 5) << AVR32_USART_MR_CHRL_OFFSET);          /* Set character length (5-8 bits). if 9-bits, set MODE9    */

    usart->cr      |=  AVR32_USART_CR_TXEN_MASK |                       /* Enable TX                                                */
                       AVR32_USART_CR_RXEN_MASK;                        /* Enable RX                                                */
}
#else /* NEW from Stephane */
void  BSP_USART_Init (CPU_INT08U com, CPU_INT32U baud_rate) {
    volatile  avr32_usart_t  *usart;
              CPU_INT32U      cd_fp;                                    /* Clock divider with fractional part                       */
              CPU_INT32U      cd;                                       /* Clock divider                                            */
              CPU_INT32U      fp;                                       /* Fractional part                                          */
              CPU_INT32U      pba_freq;


    switch (com) {
        case 0:
             BSP_GPIO_SetFnct(BSP_USART0_RXD_PIN, BSP_USART0_RXD_FUNCTION);
             BSP_GPIO_SetFnct(BSP_USART0_TXD_PIN, BSP_USART0_TXD_FUNCTION);

             usart  = &AVR32_USART0;

             break;

        case 1:
             BSP_GPIO_SetFnct(BSP_USART1_RXD_PIN, BSP_USART1_RXD_FUNCTION);
             BSP_GPIO_SetFnct(BSP_USART1_TXD_PIN, BSP_USART1_TXD_FUNCTION);

             usart  = &AVR32_USART1;

             break;

        case 2:
             BSP_GPIO_SetFnct(BSP_USART2_RXD_PIN, BSP_USART2_RXD_FUNCTION);
             BSP_GPIO_SetFnct(BSP_USART2_TXD_PIN, BSP_USART2_TXD_FUNCTION);

             usart  = &AVR32_USART2;

             break;

        case 3:
             BSP_GPIO_SetFnct(BSP_USART3_RXD_PIN, BSP_USART3_RXD_FUNCTION);
             BSP_GPIO_SetFnct(BSP_USART3_TXD_PIN, BSP_USART3_TXD_FUNCTION);

             usart  = &AVR32_USART3;

             break;

        default:
             return;
    }

    usart->idr      =  0xFFFFFFFF;                                      /* Disable all USART interrupts                             */

    usart->mr       =  0;                                               /* Reset MODE register                                      */
    usart->rtor     =  0;                                               /* Reset Receiver Time-out register                         */
    usart->ttgr     =  0;                                               /* Reset Transmitter Timeguard register                     */

    usart->cr       =  AVR32_USART_CR_RSTRX_MASK   |                    /* Reset RX                                                 */
                       AVR32_USART_CR_RSTTX_MASK   |                    /* Reset TX                                                 */
                       AVR32_USART_CR_RSTSTA_MASK  |                    /* Reset Status Bits                                        */
                       AVR32_USART_CR_RSTIT_MASK   |                    /* Reset Iterations                                         */
                       AVR32_USART_CR_RSTNACK_MASK |                    /* Reset Non Acknowledge                                    */
                       AVR32_USART_CR_DTRDIS_MASK  |                    /* Disable DTR                                              */
                       AVR32_USART_CR_RTSDIS_MASK;                      /* Disable RTS                                              */

    pba_freq        =  BSP_PBA_ClkFreq();                               /* Retrieve Peripheral Bus A operating frequency            */

    if (pba_freq < (16 * baud_rate)) {                                  /* 8x oversampling                                          */
        usart->mr  |=  AVR32_USART_MR_OVER_MASK;                        /* Set oversampling mask                                    */
        cd_fp       = ((1 << AVR32_USART_BRGR_FP_SIZE) * pba_freq +
                       (8 * baud_rate) / 2) /
                      (8 * baud_rate);
    } else {                                                            /* 16x oversampling                                         */
        usart->mr  &= ~AVR32_USART_MR_OVER_MASK;                        /* Clear oversampling mask                                  */
        cd_fp       = ((1 << AVR32_USART_BRGR_FP_SIZE) * pba_freq +
                       (16 * baud_rate) / 2) /
                      (16 * baud_rate);
    }

    cd              =  cd_fp >> AVR32_USART_BRGR_FP_SIZE;
    fp              =  cd_fp & ((1 << AVR32_USART_BRGR_FP_SIZE) - 1);

    usart->brgr     = (cd << AVR32_USART_BRGR_CD_OFFSET) |              /* Set CR on Baud Rate Generator register                   */
                      (fp << AVR32_USART_BRGR_FP_OFFSET);
                                                                        /* Set normal RS232 mode, no parity bit, 1 stop bit,        */
    usart->mr      |= (AVR32_USART_MR_MODE_NORMAL << AVR32_USART_MR_CHMODE_OFFSET) |
                      (AVR32_USART_MR_PAR_NONE    << AVR32_USART_MR_PAR_OFFSET)    |
                      (AVR32_USART_MR_NBSTOP_1    << AVR32_USART_MR_NBSTOP_OFFSET) |
                      ((8 - 5) << AVR32_USART_MR_CHRL_OFFSET);          /* Set character length (5-8 bits). if 9-bits, set MODE9    */

    usart->cr      |=  AVR32_USART_CR_TXEN_MASK |                       /* Enable TX                                                */
                       AVR32_USART_CR_RXEN_MASK;                        /* Enable RX                                                */
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                                BSP, USART - Write Byte
*
* Description :   This function writes a single byte to a serial port. Note, this function
*                 also blocks until room is available in the USART for the byte to be sent.
*
* Arguments   :   com   USART port: 0..3
*                 b     byte containing the value of the character to output.
*
* Returns     :   None.
*********************************************************************************************************
*/

void  BSP_USART_ByteWr (CPU_INT08U com, CPU_INT08U b)
{
    volatile  avr32_usart_t  *usart;


    switch (com) {
        case 0:
             usart  = &AVR32_USART0;

             break;

        case 1:
             usart  = &AVR32_USART1;

             break;

        case 2:
             usart  = &AVR32_USART2;

             break;

        case 3:
             usart  = &AVR32_USART3;

             break;

        default:
             return;
    }

    while ((usart->csr & AVR32_USART_CSR_TXRDY_MASK) == 0){             /*  Wait for room in the transmit register.                 */
        ;
    }

    usart->thr      = b;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                                BSP, USART - Read Byte
*
* Description :   This function reads a byte from a serial port. This call blocks until a
*                 character appears at the port. Echo of the byte is also sent to the serial port.
*
* Arguments   :   com   USART port: 0..3
*
* Returns     :   A byte containing the value of the received character.
*********************************************************************************************************
*/

CPU_INT08U BSP_USART_ByteRd (CPU_INT08U com)
{
    volatile  avr32_usart_t  *usart;
              CPU_INT08U      b;


    switch (com) {
        case 0:
             usart  = &AVR32_USART0;

             break;

        case 1:
             usart  = &AVR32_USART1;

             break;

        case 2:
             usart  = &AVR32_USART2;

             break;

        case 3:
             usart  = &AVR32_USART3;

             break;

        default:
             return (0);
    }

    while ((usart->csr & AVR32_USART_CSR_RXRDY_MASK) == 0) {            /*  Wait for a byte to show up.                             */
        OSTimeDly(2);
    }

    b = (CPU_INT08U)(usart->rhr & 0x00FF);                              /* Read the character.                                      */

    return (b);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                                BSP, USART - Write String
*
* Description :   This function writes a character string to a serial port.
*
* Arguments   :   com   USART port: 0..3
*                 s     string of characters
*
* Returns     :   None.
*********************************************************************************************************
*/

void  BSP_USART_StrWr (CPU_INT08U com, CPU_INT08U *s)
{
    while (*s) {
        BSP_USART_ByteWr(com, *s++);
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                                BSP, USART - Write Formatted String
*
* Description :   Formatted output to the serial port.
*
* Arguments   :   com       USART port: 0..3
*                 format    string following the C format convention.
*
* Returns     :   None.
*********************************************************************************************************
*/

void  BSP_USART_printf (CPU_INT08U com, CPU_CHAR *format, ...)
{
    static  CPU_INT08U  buffer[80 + 1] = { 0, };
            va_list     vArgs;
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
            CPU_SR      cpu_sr;
#endif


    CPU_CRITICAL_ENTER();
    va_start(vArgs, format);
    vsnprintf((char *)buffer, sizeof(buffer) - 1, (char const *)format, vArgs);
    va_end(vArgs);
    CPU_CRITICAL_EXIT();

    BSP_USART_StrWr(com, (CPU_CHAR *)buffer);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           BSP, USART - Write Value in Hexadecimal
*
* Description :   Formatted output to the serial port.
*
* Arguments   :   com       USART port: 0..3
*                 value     value to be printed in hexadecimal.
*                 digits    number of digits to be printed.
*
* Returns     :   None.
*********************************************************************************************************
*/

void  BSP_USART_PrintHex (CPU_INT08U com, CPU_INT32U value, CPU_INT08U digits)
{
    CPU_INT08U  i;
    CPU_INT32U  mult;
    CPU_INT32U  temp;


    mult = 1;
    for (i = 0; i < (digits - 1); i++) {
        mult *= 16;
    }
    while (mult > 0) {
        temp = value / mult;

        switch (temp) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                BSP_USART_ByteWr(com, temp + '0');
                break;

            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                BSP_USART_ByteWr(com, temp - 10 + 'A');
                break;
        }

        value %= mult;
        mult  /= 16;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           BSP, USART - Write Value in Decimal
*
* Description :   Formatted output to the serial port.
*
* Arguments   :   com       USART port: 0..3
*                 value     value to be printed in decimal.
*                 digits    number of digits to be printed.
*
* Returns     :   None.
*********************************************************************************************************
*/

void  BSP_USART_PrintDec (CPU_INT08U com, CPU_INT32U value, CPU_INT08U digits)
{
    CPU_INT08U  i;
    CPU_INT32U  mult;


    mult = 1;
    for (i = 0; i < (digits - 1); i++) {
        mult *= 10;
    }

    while (mult > 0) {
        BSP_USART_ByteWr(com, value / mult + '0');
        value %= mult;
        mult  /= 10;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                                BSP, USART - Enable Interrupt
*
* Description :   This function enables interrupt on a serial port.
*
* Arguments   :   com      USART port: 0..3
*                 mask     Interrupt mask
*
* Returns     :   None.
*********************************************************************************************************
*/

void  BSP_USART_IntEn (CPU_INT08U com, CPU_INT32U mask)
{
    volatile  avr32_usart_t  *usart;


    switch (com) {
        case 0:
             usart  = &AVR32_USART0;

             break;

        case 1:
             usart  = &AVR32_USART1;

             break;

        case 2:
             usart  = &AVR32_USART2;

             break;

        case 3:
             usart  = &AVR32_USART3;

             break;

        default:
             return;
    }

    usart->ier      =  mask;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                                BSP, USART - Disable Interrupt
*
* Description :   This function disables interrupt on a serial port.
*
* Arguments   :   com      USART port: 0..3
*                 mask     Interrupt mask
*
* Returns     :   None.
*********************************************************************************************************
*/

void  BSP_USART_IntDis (CPU_INT08U com, CPU_INT32U mask)
{
    volatile  avr32_usart_t  *usart;


    switch (com) {
        case 0:
             usart  = &AVR32_USART0;

             break;

        case 1:
             usart  = &AVR32_USART1;

             break;

        case 2:
             usart  = &AVR32_USART2;

             break;

        case 3:
             usart  = &AVR32_USART3;

             break;

        default:
             return;
    }

    usart->idr      =  mask;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       BSP, COMPARE (CORE) - INITIALIZE COMPARE
*
* Description: This function is called to initialize the uC/OS-II's tick source.
*              This functions uses the COUNT and COMPARE registers to create periodic interrupt calls.
*              The COUNT register increments once every clock cycle, regardless of pipeline stalls and
*              flushes. The COUNT register can both be read and written.  The COUNT register is set to
*              zero at reset. Incrementation of the COUNT register can not be disabled.  The COUNT reg
*              will increment even though a compare interrupt is pending.
*              The COMPARE register holds a value that the COUNT register is compared against.
*              The COMPARE register can both be read and written. When the COMPARE and COUNT registers
*              match, a compare interrupt request is generated. Writing values to the COMPARE register
*              clear any pending compare interrupt requests.
*
* Arguments  : none
*
* Note(s)    : 1) The compare exception generation feature is disabled if the COMPARE register  has  a
*                 value zero. The COMPARE register is written to zero upon reset.
*********************************************************************************************************
*/

static  void  BSP_TmrInit (void)
{
    CPU_INT32U  cycle;


    cycle  =  CPU_SysReg_Get_Count();
    cycle += (CPU_CLK_FREQ() / OS_TICKS_PER_SEC);

    if (cycle == 0) {                                                   /* Write 1 if cycle is 0, Note 1.                           */
        cycle++;
    }

    CPU_SysReg_Set_Compare(cycle);
}


/*
*********************************************************************************************************
*                                       BSP, COMPARE (CORE) - RELOAD COMPARE
*
* Description: This function is called to update the uC/OS-II's tick source.
*
* Arguments  : none
*
* Note(s)    : The tick is updated by incrementing the previous compare register. This way the clock
*              tick does not get skewed after every register update.
*********************************************************************************************************
*/

static  void  BSP_TmrReload (void)
{
    CPU_INT32U  cycle;


                                                                        /* Replaced timer reload reference from Compare to ...      */
                                                                        /* ... Count to avoid missing a tick interrupt due to ...   */
                                                                        /* ... accumulated error.                                   */
#if 0
    cycle  =  CPU_SysReg_Get_Compare();
#else
    cycle  =  CPU_SysReg_Get_Count();
#endif
    cycle += (CPU_CLK_FREQ() / OS_TICKS_PER_SEC);

    if (cycle == 0) {                                                   /* Write 1 if cycle is 0, Note 1.                           */
        cycle++;
    }

    CPU_SysReg_Set_Compare(cycle);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       TICK ISR
*
* Description: This function is the periodical time source of the uC/OS-II.
*
* Arguments  : none
*
* Note(s)    : 1) BSP_TmrTickISR() MUST:
*                      a) schedule next timer compare interrupt,
*                      b) call OSTimeTick().
*********************************************************************************************************
*/

void  BSP_TmrTickISR (void)
{
    BSP_TmrReload();                                                    /* Schedule next timer interrupt                            */
    OSTimeTick();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/Probe, USART ISR
*
* Description: This function is the USART interrupt.
*
* Arguments  : none
*
* Note(s)    : BSP_USARTRxTxISR() MUST check if interrupt was caused by receiving or transmitting and
*              call the proper Probe ISR handler.
*********************************************************************************************************
*/
#if uC_PROBE_COM_MODULE > 0                                                  /* Do not compile this section if Probe is disabled    */
void  BSP_USARTRxTxISR (void)
{
    volatile  avr32_usart_t  *usart;


#if PROBE_RS232_COMM_SEL == PROBE_RS232_UART_0
    usart = &AVR32_USART0;
#endif
#if PROBE_RS232_COMM_SEL == PROBE_RS232_UART_1
    usart = &AVR32_USART1;
#endif
#if PROBE_RS232_COMM_SEL == PROBE_RS232_UART_2
    usart = &AVR32_USART2;
#endif
#if PROBE_RS232_COMM_SEL == PROBE_RS232_UART_3
    usart = &AVR32_USART3;
#endif

    if (usart->CSR.rxrdy == 1) {
        ProbeRS232_RxISRHandler();

    }
    if (usart->CSR.txrdy == 1) {
        ProbeRS232_TxISRHandler();
    }
}
#endif                                                                  /* #if uC_PROBE_COM_MODULE > 0                              */


/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/Probe, INITIALIZE TIMER
*
* Description: This function initializes a 16 or 32-bit free-running timer for uC/Probe.
*
* Arguments  : none
*
* Note(s)    : 1) AVR32 has a COUNT register that increments once every clock cycle, regardless of pipeline
*                 stalls and flushes. Thus, there is no need for a dedicated TIMER/COUNTER for uC/Probe.
*********************************************************************************************************
*/
#if OS_PROBE_HOOKS_EN > 0                                               /* Do not compile this section if Probe is disabled         */
void  OSProbe_TmrInit (void)
{
}
#endif                                                                  /* #if OS_PROBE_HOOKS_EN > 0                                */

/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/Probe, TIMER READ
*
* Description: This function reads a 16 or 32-bit free-running timer for uC/Probe.
*
* Arguments  : none
*
* Returns    : The current COUNT register value with the amount of elapsed clock cycles since reset.
*********************************************************************************************************
*/
#if OS_PROBE_HOOKS_EN > 0                                               /* Do not compile this section if Probe is disabled         */
CPU_INT32U  OSProbe_TmrRd (void)
{
    return(CPU_SysReg_Get_Count());
}
#endif                                                                  /* #if OS_PROBE_HOOKS_EN > 0                                */


/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View, USART ISR
*
* Description: This function is the USART interrupt.
*
* Arguments  : none
*
* Note(s)    : BSP_USARTRxTxISR() MUST check if interrupt was caused by receiving or transmitting and call
*              the proper OSView ISR handler.
*********************************************************************************************************
*/
#if OS_VIEW_MODULE > 0                                                  /* Do not compile this section if OS-View is disabled       */
void  BSP_USARTRxTxISR (void)
{
    volatile  avr32_usart_t  *usart;


#if OS_VIEW_COMM_SEL == OS_VIEW_UART_0
    usart = &AVR32_USART0;
#endif
#if OS_VIEW_COMM_SEL == OS_VIEW_UART_1
    usart = &AVR32_USART1;
#endif
#if OS_VIEW_COMM_SEL == OS_VIEW_UART_2
    usart = &AVR32_USART2;
#endif
#if OS_VIEW_COMM_SEL == OS_VIEW_UART_3
    usart = &AVR32_USART3;
#endif

    if (usart->CSR.rxrdy == 1) {
        OSView_RxISRHandler();

    }
    if (usart->CSR.txrdy == 1) {
        OSView_TxISRHandler();
    }
}
#endif                                                                  /* #if OS_VIEW_MODULE > 0                                   */

/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View, INITIALIZE COM PORT
*
* Description: This function initializes the COM port.
*
* Arguments  : baud_rate    com port baudrate in bps.
*
* Note(s)    : It sets up RS232 {baud_rate} 8N1
*********************************************************************************************************
*/
#if OS_VIEW_MODULE > 0                                                  /* Do not compile this section if OS-View is disabled       */
void  OSView_UARTInit (CPU_INT32U baud_rate)
{
    BSP_USART_Init(OS_VIEW_COMM_SEL, baud_rate);
}
#endif                                                                  /* #if OS_VIEW_MODULE > 0                                   */

/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View, INITIALIZE TIMER
*
* Description: This function initializes a 16 or 32-bit free-running timer for uC/OS-View.
*
* Arguments  : none
*
* Note(s)    : 1) AVR32 has a COUNT register that increments once every clock cycle, regardless of pipeline
*                 stalls and flushes. Thus, there is no need for a dedicated TIMER/COUNTER for uC/OS-View.
*********************************************************************************************************
*/
#if OS_VIEW_MODULE > 0                                                  /* Do not compile this section if OS-View is disabled       */
void  OSView_TmrInit (void)
{
}
#endif                                                                  /* #if OS_VIEW_MODULE > 0                                   */

/*
*********************************************************************************************************
*                                       uC/OS-View, TIMER READ
*
* Description: This function reads a 16 or 32-bit free-running timer for uC/OS-View.
*
* Arguments  : none
*
* Returns    : The current COUNT register value with the amount of elapsed clock cycles since reset.
*********************************************************************************************************
*/
#if OS_VIEW_MODULE > 0                                                  /* Do not compile this section if OS-View is disabled       */
CPU_INT32U  OSView_TmrRd (void)
{
    return(CPU_SysReg_Get_Count());
}
#endif                                                                  /* #if OS_VIEW_MODULE > 0                                   */

/*
*********************************************************************************************************
*                                       LED INITIALIZATION
*
* Description: This function is used to initialize the LEDs on the board.
*
* Arguments  : none
*********************************************************************************************************
*/

static  void  LED_Init (void)
{
    LED_Off(0);                                                         /* Turn ON all the LEDs                                     */
}


/*$PAGE*/
