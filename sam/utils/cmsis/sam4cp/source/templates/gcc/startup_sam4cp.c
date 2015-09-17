/**
 * \file
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "sam4cp.h"

#if __FPU_USED /* CMSIS defined value to indicate usage of FPU */
#include "fpu.h"
#endif

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

void __libc_init_array(void);

/* Default empty handler */
void Dummy_Handler(void);

/* Cortex-M4 core handlers */
void NMI_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void HardFault_Handler  ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void MemManage_Handler  ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void BusFault_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UsageFault_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SVC_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DebugMon_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PendSV_Handler     ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SysTick_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

/* Peripherals handlers */
void SUPC_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RSTC_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RTC_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RTT_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void WDT_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PMC_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EFC_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UART0_Handler  ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SMC0_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOA_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOB_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART0_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART1_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART2_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART3_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART4_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWI0_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWI1_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SPI0_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC0_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC2_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC3_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC4_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC5_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ADC_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ARM1_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void IPC0_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SLCDC_Handler  ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TRNG_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ICM_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void CPKCC_Handler  ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void AES_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOC_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UART1_Handler  ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void IPC1_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SPI1_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PWM_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

/* Exception Table */
__attribute__ ((section(".vectors")))
const DeviceVectors exception_table = {

        /* Configure Initial Stack Pointer, using linker-generated symbols */
        .pvStack = (void*) (&_estack),

        .pfnReset_Handler      = (void*) Reset_Handler,
        .pfnNMI_Handler        = (void*) NMI_Handler,
        .pfnHardFault_Handler  = (void*) HardFault_Handler,
        .pfnMemManage_Handler  = (void*) MemManage_Handler,
        .pfnBusFault_Handler   = (void*) BusFault_Handler,
        .pfnUsageFault_Handler = (void*) UsageFault_Handler,
        .pfnReserved1_Handler  = (void*) (0UL),          /* Reserved */
        .pfnReserved2_Handler  = (void*) (0UL),          /* Reserved */
        .pfnReserved3_Handler  = (void*) (0UL),          /* Reserved */
        .pfnReserved4_Handler  = (void*) (0UL),          /* Reserved */
        .pfnSVC_Handler        = (void*) SVC_Handler,
        .pfnDebugMon_Handler   = (void*) DebugMon_Handler,
        .pfnReserved5_Handler  = (void*) (0UL),          /* Reserved */
        .pfnPendSV_Handler     = (void*) PendSV_Handler,
        .pfnSysTick_Handler    = (void*) SysTick_Handler,

        /* Configurable interrupts */
        .pfnSUPC_Handler   = (void*) SUPC_Handler,   /* 0  Supply Controller */
        .pfnRSTC_Handler   = (void*) RSTC_Handler,   /* 1  Reset Controller */
        .pfnRTC_Handler    = (void*) RTC_Handler,    /* 2  Real Time Clock */
        .pfnRTT_Handler    = (void*) RTT_Handler,    /* 3  Real Time Timer */
        .pfnWDT_Handler    = (void*) WDT_Handler,    /* 4  Watchdog Timer */
        .pfnPMC_Handler    = (void*) PMC_Handler,    /* 5  Power Management Controller */
        .pfnEFC_Handler    = (void*) EFC_Handler,    /* 6  Enhanced Embedded Flash Controller */
        .pvReserved7       = (void*) (0UL),          /* 7  Reserved */
        .pfnUART0_Handler  = (void*) UART0_Handler,  /* 8  UART 0 */
        .pvReserved9       = (void*) (0UL),          /* 9  Reserved */
        .pfnSMC0_Handler   = (void*) SMC0_Handler,   /* 10 Static Memory Controller 0 */
        .pfnPIOA_Handler   = (void*) PIOA_Handler,   /* 11 Parallel I/O Controller A */
        .pfnPIOB_Handler   = (void*) PIOB_Handler,   /* 12 Parallel I/O Controller B */
        .pvReserved13      = (void*) (0UL),          /* 13 Reserved */
        .pfnUSART0_Handler = (void*) USART0_Handler, /* 14 USART 0 */
        .pfnUSART1_Handler = (void*) USART1_Handler, /* 15 USART 1 */
        .pfnUSART2_Handler = (void*) USART2_Handler, /* 16 USART 2 */
        .pfnUSART3_Handler = (void*) USART3_Handler, /* 17 USART 3 */
        .pfnUSART4_Handler = (void*) USART4_Handler, /* 18 USART 4 */
        .pfnTWI0_Handler   = (void*) TWI0_Handler,   /* 19 Two Wire Interface 0 */
        .pfnTWI1_Handler   = (void*) TWI1_Handler,   /* 20 Two Wire Interface 1 */
        .pfnSPI0_Handler   = (void*) SPI0_Handler,   /* 21 Serial Peripheral Interface 0 */
        .pvReserved22      = (void*) (0UL),          /* 22 Reserved */
        .pfnTC0_Handler    = (void*) TC0_Handler,    /* 23 Timer/Counter 0 */
        .pfnTC1_Handler    = (void*) TC1_Handler,    /* 24 Timer/Counter 1 */
        .pfnTC2_Handler    = (void*) TC2_Handler,    /* 25 Timer/Counter 2 */
        .pfnTC3_Handler    = (void*) TC3_Handler,    /* 26 Timer/Counter 3 */
        .pfnTC4_Handler    = (void*) TC4_Handler,    /* 27 Timer/Counter 4 */
        .pfnTC5_Handler    = (void*) TC5_Handler,    /* 28 Timer/Counter 5 */
        .pfnADC_Handler    = (void*) ADC_Handler,    /* 29 Analog To Digital Converter */
        .pfnARM1_Handler   = (void*) ARM1_Handler,   /* 30 FPU signals: FPIXC, FPOFC, FPUFC, FPIOC, FPDZC, FPIDC, FPIXC */
        .pfnIPC0_Handler   = (void*) IPC0_Handler,   /* 31 Interprocessor communication 0 */
        .pfnSLCDC_Handler  = (void*) SLCDC_Handler,  /* 32 Segment LCD Controller */
        .pfnTRNG_Handler   = (void*) TRNG_Handler,   /* 33 True Random Generator */
        .pfnICM_Handler    = (void*) ICM_Handler,    /* 34 Integrity Check Module */
        .pfnCPKCC_Handler  = (void*) CPKCC_Handler,  /* 35 Public Key Cryptography Controller */
        .pfnAES_Handler    = (void*) AES_Handler,    /* 36 Advanced Enhanced Standard */
        .pfnPIOC_Handler   = (void*) PIOC_Handler,   /* 37 Parallel I/O Controller C */
        .pfnUART1_Handler  = (void*) UART1_Handler,  /* 38 UART 1 */
        .pfnIPC1_Handler   = (void*) IPC1_Handler,   /* 39 Interprocessor communication 1 */
        .pfnSPI1_Handler   = (void*) SPI1_Handler,   /* 40 Serial Peripheral Interface 1 */
        .pfnPWM_Handler    = (void*) PWM_Handler,    /* 41 Pulse Width Modulation */
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
	uint32_t *pSrc, *pDest;

	/* Initialize the relocate segment */
	pSrc = &_etext;
	pDest = &_srelocate;

	if (pSrc > pDest) {
		/* Copy segment block from beginning to end */
		for (; pDest < &_erelocate;) {
			*pDest++ = *pSrc++;
		}
	} else if (pSrc < pDest) {
		/* Copy segment block from end to beginning */
		uint32_t bytes_relocate = (uint32_t)&_erelocate - (uint32_t)&_srelocate;
		pSrc = (uint32_t*)((uint32_t)pSrc + bytes_relocate) - 1;
		pDest = (uint32_t*)((uint32_t)pDest + bytes_relocate) - 1;

		for (; bytes_relocate; bytes_relocate -= 4) {
			*pDest-- = *pSrc--;
		}
	}

	/* Clear the zero segment */
	for (pDest = &_szero; pDest < &_ezero;) {
		*pDest++ = 0;
	}

	/* Set the vector table base address */
	pSrc = (uint32_t *) & _sfixed;
	SCB->VTOR = (uint32_t)pSrc;

#if __FPU_USED
	/* Enable FPU */
	fpu_enable();
#endif

	/* Initialize the C library */
	__libc_init_array();

	/* Branch to main function */
	main();

	/* Infinite loop */
	while (1);
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
        while (1) {
        }
}
