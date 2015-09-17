/**
 * \file
 *
 * \brief AT86RFX configuration
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
#ifndef CONF_AT86RFX_H
#define CONF_AT86RFX_H

#if defined(DOXYGEN)
/**
 * Attribute to apply to an enum declaration to force it into the smallest
 * type size required to represent all values.
 */
#define SHORTENUM
/**
 * Null operation: just waste one CPU cycle.
 */
#define nop()
#endif /* defined(DOXYGEN) */

#if defined(__ICCAVR32__)
#define SHORTENUM /**/
#define nop() __no_operation()
#endif /* defined(__ICCAVR32__) */
#if defined(__GNUC__)
#define SHORTENUM           __attribute__((packed))
#define nop() do { __asm__ __volatile__ ("nop"); } while (0)
#endif /* defined(__GNUC__) */

/*! \name Macros for IRQ and AT86RFX accessing GPIO
 */
//! @{
#define EXT_INT                         (AVR32_PIN_PA20)
#define EXT_INT_ISR_GROUP               2
#define EXT_INT_ISR_PRIORITY            1

 //! RESET pin of transceiver
#define TRX_RST                         (AVR32_PIN_PA17)
 //! Sleep Transceiver pin
#define SLP_TR                          (AVR32_PIN_PA19)
//! @}

/**
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION()              DISABLE_TRX_IRQ()
/**
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()              ENABLE_TRX_IRQ()

#if (UC3)
/* This macro saves the global interrupt status */
#define ENTER_CRITICAL_REGION()                 AVR32_ENTER_CRITICAL_REGION( )

/* This macro restores the global interrupt status */
#define LEAVE_CRITICAL_REGION()                 AVR32_LEAVE_CRITICAL_REGION( )

/* Wait for 65 ns. */
#define  WAIT_65_NS()  {nop(); nop();}

/* Wait for 1 us. Has to be provided based on the controller interfaced */
#define PAL_WAIT_1_US()      do {              \
} while (0)

/* Wait for 1 us. Has to be provided based on the controller interfaced */
#define DELAY_US(delay)      do {              \
} while (0)
#endif

/**
 * AT86RFX Module Callbacks definitions (Optional)
 * @{
 */
#define AT86RFX_RX_NOTIFY(frame)
#define AT86RFX_TX_STATUS_NOTIFY(status)
//@}

#if !defined(AT86RFX_SPI)              || \
    !defined(AT86RFX_SPI_NPCS)         || \
    !defined(AT86RFX_SPI_SCK_PIN)      || \
    !defined(AT86RFX_SPI_SCK_FUNCTION) || \
    !defined(AT86RFX_SPI_MISO_PIN)     || \
    !defined(AT86RFX_SPI_MISO_FUNCTION)|| \
    !defined(AT86RFX_SPI_MOSI_PIN)     || \
    !defined(AT86RFX_SPI_MOSI_FUNCTION)|| \
    !defined(AT86RFX_SPI_NPCS_PIN)     || \
    !defined(AT86RFX_SPI_NPCS_FUNCTION)|| \
    !defined(AT86RFX_SPI_MISO_PIN)     || \
    !defined(AT86RFX_SPI_BAUDRATE) 
#  warning The AT86RFX SPI setups configuration to use in the driver is missing. Default configuration is used.
#  define AT86RFX_SPI                  (&AVR32_SPI0)
#  define AT86RFX_SPI_NPCS             0
#  define AT86RFX_SPI_SCK_PIN          AVR32_SPI0_SCK_0_0_PIN
#  define AT86RFX_SPI_SCK_FUNCTION     AVR32_SPI0_SCK_0_0_FUNCTION
#  define AT86RFX_SPI_MISO_PIN         AVR32_SPI0_MISO_0_0_PIN
#  define AT86RFX_SPI_MISO_FUNCTION    AVR32_SPI0_MISO_0_0_FUNCTION
#  define AT86RFX_SPI_MOSI_PIN         AVR32_SPI0_MOSI_0_0_PIN
#  define AT86RFX_SPI_MOSI_FUNCTION    AVR32_SPI0_MOSI_0_0_FUNCTION
#  define AT86RFX_SPI_NPCS_PIN         AVR32_SPI0_NPCS_0_0_PIN
#  define AT86RFX_SPI_NPCS_FUNCTION    AVR32_SPI0_NPCS_0_0_FUNCTION
#  define AT86RFX_SPI_BAUDRATE         (48000)
#endif

#endif /* CONF_AT86RFX_H */
