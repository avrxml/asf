/**
 * @file trx_access_2.h
 *
 * @brief HAL related APIs for externally plugged transceivers
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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
 *
 */

/*
 * Copyright (c) 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef TRX_ACCESS_2_H
#define TRX_ACCESS_2_H

/*
 * NOTE:- Include 'return_val.h' before this file, as return_val.h has the
 *        all return value enums.
 */

/**
 * \defgroup group_trx_access
 *  This module includes api's and defenitions required for Devices with
 * externally plugged transceivers(Non Soc's)
 * @{
 */
/* === Includes ============================================================ */

#include "compiler.h"
#include "conf_trx_access.h"
#if SAMD || SAMR21
#include "port.h"
#include "extint.h"
#else
#include "ioport.h"
#endif
#include "board.h"

/* === Macros =============================================================== */

/**
 * Write access command of the transceiver
 */
#define WRITE_ACCESS_COMMAND            (0x8000)

/**
 * Read access command to the tranceiver
 */
#define READ_ACCESS_COMMAND             (0x0000)


#define TRX_TRIG_DELAY()  {nop(); nop(); }

/**
 * Set TRX GPIO pins.
 */
#if SAMD || SAMR21
#define RST_HIGH()                      port_pin_set_output_level( \
		AT86RFX_RST_PIN, true)
#define RST_LOW()                       port_pin_set_output_level( \
		AT86RFX_RST_PIN, false)
#define SLP_TR_HIGH()                   port_pin_set_output_level( \
		AT86RFX_SLP_PIN, true)
#define SLP_TR_LOW()                    port_pin_set_output_level( \
		AT86RFX_SLP_PIN, false)
#define IRQ_PINGET()                    port_pin_get_input_level(AT86RFX_IRQ_PIN)
#else
#define RST_HIGH()                      ioport_set_pin_level(AT86RFX_RST_PIN, \
		HIGH)
#define RST_LOW()                       ioport_set_pin_level(AT86RFX_RST_PIN, \
		LOW)
#define SLP_TR_HIGH()                   ioport_set_pin_level(AT86RFX_SLP_PIN, \
		HIGH)
#define SLP_TR_LOW()                    ioport_set_pin_level(AT86RFX_SLP_PIN, \
		LOW)
#define IRQ_PINGET()                    ioport_get_pin_level(AT86RFX_IRQ_PIN)
#endif

/**
 * @brief Clears the transceiver main interrupt
 *
 */
#define trx_irq_flag_clr()          CLEAR_TRX_IRQ()

/**
 * This macro is used for handling endianness among the different CPUs.
 */
#if (UC3)
#define U16_TO_TARGET(x) ((((x) << 8) & 0xFF00) | (((x) >> 8) & 0x00FF))
#else
#define U16_TO_TARGET(x) (x)
#endif

/**
 * @brief Transceiver ISR handler
 */
typedef void (*irq_handler_t)(void);
/* === Types =============================================================== */

/*
 * The smallest timeout in microseconds
 */
/* #define MIN_TIMEOUT                     (0x80) */

/* This macro saves the global interrupt status */
#define ENTER_TRX_CRITICAL_REGION()              {uint8_t flags	\
							  = cpu_irq_save();

/* This macro restores the global interrupt status */
#define LEAVE_TRX_CRITICAL_REGION()              cpu_irq_restore(flags); }

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

void trx_irq_init(FUNC_PTR trx_irq_cb);

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Macros for TRX GPIO pins access.
 */
/** Macro to set Reset pin to high */
#define TRX_RST_HIGH()                  RST_HIGH()
/** Macro to set Reset pin to low */
#define TRX_RST_LOW()                   RST_LOW()
/** Macro to set SLP_TR pin to high */
#define TRX_SLP_TR_HIGH()               SLP_TR_HIGH()
/** Macro to set SLP_TR pin to low */
#define TRX_SLP_TR_LOW()                SLP_TR_LOW()
/** Macro to get the transceiver's main IRQ status */
#define TRX_IRQ_HIGH()                  IRQ_PINGET()

#define TRX_IRQ_GET()                               IRQ_PINGET()

uint8_t trx_reg_read(uint16_t addr);

/**
 * @brief Writes data into a transceiver register
 *
 * This function writes a value into transceiver register.
 *
 * @param addr Address of the trx register
 * @param data Data to be written to trx register
 *
 */
void trx_reg_write(uint16_t addr, uint8_t data);

void trx_write(uint16_t addr, uint8_t *data, uint16_t length);

void trx_read(uint16_t addr, uint8_t *data, uint16_t length);

/**
 * @brief Subregister read
 *
 * @param   addr  offset of the register
 * @param   mask  bit mask of the subregister
 * @param   pos   bit position of the subregister
 *
 * @return  value of the read bit(s)
 */
uint8_t trx_bit_read(uint16_t addr, uint8_t mask, uint8_t pos);

/**
 * @brief Subregister write
 *
 * @param[in]   reg_addr  Offset of the register
 * @param[in]   mask  Bit mask of the subregister
 * @param[in]   pos   Bit position of the subregister
 * @param[out]  new_value  Data, which is muxed into the register
 */
void trx_bit_write(uint16_t reg_addr, uint8_t mask, uint8_t pos,
		uint8_t new_value);

#if defined(NON_BLOCKING_SPI) || defined(__DOXYGEN__)

/**
 * @brief SPI done callback initialization
 *
 * @param spi_done_cb Pointer to SPI done callback function
 */
void trx_spi_done_cb_init(void *spi_done_cb);

#endif

/**
 * @brief Initializes the SPI interface for communication with the transceiver
 */
void trx_spi_init(void);

/**
 * @brief Resets the TRX radio
 */
void PhyReset(void);

/* ! @} */
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* TRX_ACCESS_H */
/* EOF */
