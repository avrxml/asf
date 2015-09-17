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

#include "samd10.h"

void __iar_program_start(void);
int __low_level_init(void);

void Dummy_Handler(void);
void Reset_Handler(void);

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
        while (1) {
        }
}

/* Cortex-M0+ core handlers */
void NMI_Handler              ( void );
void HardFault_Handler        ( void );
void SVC_Handler              ( void );
void PendSV_Handler           ( void );
void SysTick_Handler          ( void );

/* Peripherals handlers */
void PM_Handler               ( void );
void SYSCTRL_Handler          ( void );
void WDT_Handler              ( void );
void RTC_Handler              ( void );
void EIC_Handler              ( void );
void NVMCTRL_Handler          ( void );
void DMAC_Handler             ( void );
void USB_Handler              ( void );
void EVSYS_Handler            ( void );
void SERCOM0_Handler          ( void );
void SERCOM1_Handler          ( void );
void SERCOM2_Handler          ( void );
void TCC0_Handler             ( void );
void TC1_Handler              ( void );
void TC2_Handler              ( void );
void ADC_Handler              ( void );
void AC_Handler               ( void );
void DAC_Handler              ( void );
void PTC_Handler              ( void );

/* Cortex-M0+ core handlers */
#pragma weak NMI_Handler              = Dummy_Handler
#pragma weak HardFault_Handler        = Dummy_Handler
#pragma weak SVC_Handler              = Dummy_Handler
#pragma weak PendSV_Handler           = Dummy_Handler
#pragma weak SysTick_Handler          = Dummy_Handler

/* Peripherals handlers */
#pragma weak PM_Handler               = Dummy_Handler
#pragma weak SYSCTRL_Handler          = Dummy_Handler
#pragma weak WDT_Handler              = Dummy_Handler
#pragma weak RTC_Handler              = Dummy_Handler
#pragma weak EIC_Handler              = Dummy_Handler
#pragma weak NVMCTRL_Handler          = Dummy_Handler
#pragma weak DMAC_Handler             = Dummy_Handler
#pragma weak USB_Handler              = Dummy_Handler
#pragma weak EVSYS_Handler            = Dummy_Handler
#pragma weak SERCOM0_Handler          = Dummy_Handler
#pragma weak SERCOM1_Handler          = Dummy_Handler
#pragma weak SERCOM2_Handler          = Dummy_Handler
#pragma weak TCC0_Handler             = Dummy_Handler
#pragma weak TC1_Handler              = Dummy_Handler
#pragma weak TC2_Handler              = Dummy_Handler
#pragma weak ADC_Handler              = Dummy_Handler
#pragma weak AC_Handler               = Dummy_Handler
#pragma weak DAC_Handler              = Dummy_Handler
#pragma weak PTC_Handler              = Dummy_Handler

/* Exception Table */
#pragma language=extended
#pragma segment="CSTACK"

/* The name "__vector_table" has special meaning for C-SPY: */
/* it is where the SP start value is found, and the NVIC vector */
/* table register (VTOR) is initialized to this address if != 0 */

#pragma section = ".intvec"
#pragma location = ".intvec"
//! [startup_vector_table]
const DeviceVectors __vector_table[] = {
        __sfe("CSTACK"),
        (void*) Reset_Handler,
        (void*) NMI_Handler,
        (void*) HardFault_Handler,
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) SVC_Handler,
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) PendSV_Handler,
        (void*) SysTick_Handler,

        /* Configurable interrupts */
        (void*) PM_Handler,             /*  0 Power Manager */
        (void*) SYSCTRL_Handler,        /*  1 System Control */
        (void*) WDT_Handler,            /*  2 Watchdog Timer */
        (void*) RTC_Handler,            /*  3 Real-Time Counter */
        (void*) EIC_Handler,            /*  4 External Interrupt Controller */
        (void*) NVMCTRL_Handler,        /*  5 Non-Volatile Memory Controller */
        (void*) DMAC_Handler,           /*  6 Direct Memory Access Controller */
        (void*) USB_Handler,            /*  7 Universal Serial Bus */
        (void*) EVSYS_Handler,          /*  8 Event System Interface */
        (void*) SERCOM0_Handler,        /*  9 Serial Communication Interface 0 */
        (void*) SERCOM1_Handler,        /* 10 Serial Communication Interface 1 */
        (void*) SERCOM2_Handler,        /* 11 Serial Communication Interface 2 */
        (void*) TCC0_Handler,           /* 12 Timer Counter Control 0 */
        (void*) TC1_Handler,            /* 13 Basic Timer Counter 0 */
        (void*) TC2_Handler,            /* 14 Basic Timer Counter 1 */
        (void*) ADC_Handler,            /* 15 Analog Digital Converter */
        (void*) AC_Handler,             /* 16 Analog Comparators */
        (void*) DAC_Handler,            /* 17 Digital Analog Converter */
        (void*) PTC_Handler             /* 18 Peripheral Touch Controller */
};
//! [startup_vector_table]

/**------------------------------------------------------------------------------
 * This is the code that gets called on processor reset. To initialize the
 * device.
 *------------------------------------------------------------------------------*/
int __low_level_init(void)
{
        uint32_t *pSrc = __section_begin(".intvec");

        SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

        return 1; /* if return 0, the data sections will not be initialized */
}

/**------------------------------------------------------------------------------
 * This is the code that gets called on processor reset. To initialize the
 * device.
 *------------------------------------------------------------------------------*/
void Reset_Handler(void)
{
        /* Overwriting the default value of the NVMCTRL.CTRLB.MANW bit (errata reference 13134) */
        NVMCTRL->CTRLB.bit.MANW = 1;

        __iar_program_start();
}
