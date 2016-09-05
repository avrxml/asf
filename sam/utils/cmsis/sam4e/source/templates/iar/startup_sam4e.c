/**
 * \file
 *
 * \brief Startup file for SAM4E.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

#include "exceptions.h"
#include "sam4e.h"
#include "system_sam4e.h"

typedef void (*intfunc) (void);
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

void __iar_program_start(void);
int __low_level_init(void);

/* Exception Table */
#pragma language=extended
#pragma segment="CSTACK"

/* The name "__vector_table" has special meaning for C-SPY: */
/* it is where the SP start value is found, and the NVIC vector */
/* table register (VTOR) is initialized to this address if != 0 */

#pragma section = ".intvec"
#pragma location = ".intvec"
const intvec_elem __vector_table[] = {
	{.__ptr = __sfe("CSTACK")},
	{Reset_Handler},

	{NMI_Handler},
	{HardFault_Handler},
	{MemManage_Handler},
	{BusFault_Handler},
	{UsageFault_Handler},
	{0}, {0}, {0}, {0},   /*  Reserved */
	{SVC_Handler},
	{DebugMon_Handler},
	{0},                   /*  Reserved */
	{PendSV_Handler},
	{SysTick_Handler},

	/* Configurable interrupts */
	{SUPC_Handler},   /* 0  Supply Controller */
	{RSTC_Handler},   /* 1  Reset Controller */
	{RTC_Handler},    /* 2  Real Time Clock */
	{RTT_Handler},    /* 3  Real Time Timer */
	{WDT_Handler},    /* 4  Watchdog/Dual Watchdog Timer */
	{PMC_Handler},    /* 5  Power Management Controller */
	{EFC_Handler},    /* 6  Enhanced Embedded Flash Controller */
	{UART0_Handler},  /* 7  UART 0 */
	{Dummy_Handler},
	{PIOA_Handler},   /* 9  Parallel I/O Controller A */
	{PIOB_Handler},   /* 10 Parallel I/O Controller B */
#ifdef _SAM4E_PIOC_INSTANCE_
	{PIOC_Handler},   /* 11 Parallel I/O Controller C */
#else
	{Dummy_Handler},
#endif
#ifdef _SAM4E_PIOD_INSTANCE_
	{PIOD_Handler},   /* 12 Parallel I/O Controller D */
#else
	{Dummy_Handler},
#endif
#ifdef _SAM4E_PIOE_INSTANCE_
	{PIOE_Handler},   /* 13 Parallel I/O Controller E */
#else
	{Dummy_Handler},
#endif
	{USART0_Handler}, /* 14 USART 0 */
	{USART1_Handler}, /* 15 USART 1 */
	{HSMCI_Handler},  /* 16 Multimedia Card Interface */
	{TWI0_Handler},   /* 17 Two Wire Interface 0 */
	{TWI1_Handler},   /* 18 Two Wire Interface 1 */
	{SPI_Handler},    /* 19 Serial Peripheral Interface */
	{DMAC_Handler},   /* 20 DMAC */
	{TC0_Handler},    /* 21 Timer/Counter 0 */
	{TC1_Handler},    /* 22 Timer/Counter 1 */
	{TC2_Handler},    /* 23 Timer/Counter 2 */
#ifdef _SAM4E_TC1_INSTANCE_
	{TC3_Handler},    /* 24 Timer/Counter 3 */
	{TC4_Handler},    /* 25 Timer/Counter 4 */
	{TC5_Handler},    /* 26 Timer/Counter 5 */
#else
	{Dummy_Handler},
	{Dummy_Handler},
	{Dummy_Handler},
#endif
#ifdef _SAM4E_TC2_INSTANCE_
	{TC6_Handler},    /* 27 Timer/Counter 6 */
	{TC7_Handler},    /* 28 Timer/Counter 7 */
	{TC8_Handler},    /* 29 Timer/Counter 8 */
#else
	{Dummy_Handler},
	{Dummy_Handler},
	{Dummy_Handler},
#endif
	{AFEC0_Handler},  /* 30 Analog Front End 0 */
	{AFEC1_Handler},  /* 31 Analog Front End 1 */
	{DACC_Handler},   /* 32 Digital To Analog Converter */
	{ACC_Handler},    /* 33 Analog Comparator */
	{ARM_Handler},    /* 34 FPU signals : FPIXC, FPOFC, FPUFC, FPIOC, FPDZC, FPIDC, FPIXC */
	{UDP_Handler},    /* 35 USB DEVICE */
	{PWM_Handler},    /* 36 PWM */
	{CAN0_Handler},   /* 37 CAN0 */
#ifdef _SAM4E_CAN1_INSTANCE_
	{CAN1_Handler},   /* 38 CAN1 */
#else
	{Dummy_Handler},
#endif
	{AES_Handler},    /* 39 AES */
	{Dummy_Handler},
	{Dummy_Handler},
	{Dummy_Handler},
	{Dummy_Handler},
#ifdef _SAM4E_GMAC_INSTANCE_
	{GMAC_Handler},   /* 44 GMAC */
#else
	{Dummy_Handler},
#endif
	{UART1_Handler}   /* 45 UART */
};

/* EWARM 6.30 integrates CMSIS 2.10 (__CM4_CMSIS_VERSION 0x0210),
 * in which SCB_VTOR_TBLBASE_Msk not defined.
 */
#if (__VER__ >= 6030000)
/* TEMPORARY PATCH FOR SCB */
#define SCB_VTOR_TBLBASE_Pos               29                            /*!< SCB VTOR: TBLBASE Position */
#define SCB_VTOR_TBLBASE_Msk               (1UL << SCB_VTOR_TBLBASE_Pos) /*!< SCB VTOR: TBLBASE Mask */
#endif

/**------------------------------------------------------------------------------
 * This is the code that gets called on processor reset. To initialize the
 * device.
 *------------------------------------------------------------------------------*/
int __low_level_init(void)
{
	uint32_t *pSrc = __section_begin(".intvec");

	SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

	if (((uint32_t) pSrc >= IRAM_ADDR) && ((uint32_t) pSrc < (uint32_t) IRAM_ADDR + (uint32_t) IRAM_SIZE)) {
		SCB->VTOR |= (uint32_t) (1 << SCB_VTOR_TBLBASE_Pos);
	}

	return 1; /* if return 0, the data sections will not be initialized */
}

/**------------------------------------------------------------------------------
 * This is the code that gets called on processor reset. To initialize the
 * device.
 *------------------------------------------------------------------------------*/
void Reset_Handler(void)
{
	__iar_program_start();
}
